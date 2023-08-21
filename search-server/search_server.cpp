#include "search_server.h"

using namespace std;

SearchServer::SearchServer(const std::string& stop_words_text)
    : SearchServer(
        SplitIntoWords(stop_words_text))  
{
}

void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const vector<int>& ratings)
{       
        if ((document_id < 0) || (documents_.count(document_id) > 0)) {
            throw invalid_argument("Invalid document_id"s);
        }
        storage_.emplace_back(document);
        const auto words_view = SplitIntoWordsNoStop(storage_.back());
        const double inv_word_count = 1.0 / words_view.size();
        for (const string_view word : words_view) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
            word_freqs_in_document_[document_id][word] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ SearchServer::ComputeAverageRating(ratings), status });
        document_ids_.insert(document_id);
 }
//2
std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query, DocumentStatus status) const
{
    return FindTopDocuments(execution::seq,
        raw_query, [status]([[maybe_unused]] int document_id, DocumentStatus document_status, [[maybe_unused]] int rating) {
            return document_status == status;
        });
}
// 3
std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query) const
{
    return FindTopDocuments(execution::seq, raw_query, DocumentStatus::ACTUAL);
}



int SearchServer::GetDocumentCount() const
{
    return static_cast<int>(documents_.size());
}

set<int>::iterator SearchServer::begin() 
{
    return document_ids_.begin();
}

set<int>::iterator SearchServer::end() 
{
    return document_ids_.end();
}

const map<string_view, double>& SearchServer::GetWordFrequencies(int document_id) const
{
    
    if (word_freqs_in_document_.count(document_id) == 0) {
    static const map<std::string_view, double> empty;
            return empty;
    }              
    return word_freqs_in_document_.at(document_id);;
}

void SearchServer::RemoveDocument(int document_id)
{
    RemoveDocument(execution::seq, document_id);
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(const string_view& raw_query, int document_id) const
{
    return MatchDocument(execution::seq, raw_query, document_id);
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::sequenced_policy&, const std::string_view& raw_query, int document_id) const
{
    if (word_freqs_in_document_.count(document_id) == 0) {
        throw std::invalid_argument("Invalid Argument");
    }
    if (document_ids_.count(document_id) == 0) {
        throw std::out_of_range("Out of range");
    }

    const auto query = SearchServer::ParseQuery(raw_query, true);

    std::vector<std::string_view> matched_words(query.plus_words.size());

    auto pred = [this, document_id](const auto& word) {return word_freqs_in_document_.at(document_id).count(word); };

    if (std::any_of(query.minus_words.begin(), query.minus_words.end(), pred)) {
        matched_words.clear();
        return { matched_words, documents_.at(document_id).status };
    }

    auto end_copy = std::copy_if(query.plus_words.begin(), query.plus_words.end(), matched_words.begin(), pred);
    matched_words.erase(end_copy, matched_words.end());

    return { matched_words, documents_.at(document_id).status };
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::parallel_policy&, const std::string_view& raw_query, int document_id) const
{
    if (word_freqs_in_document_.count(document_id) == 0) {
        throw std::invalid_argument("Invalid Argument");
    }
    if (document_ids_.count(document_id) == 0) {
        throw std::out_of_range("Out of range");
    }

    const auto query = SearchServer::ParseQuery(raw_query, false);
    std::vector<std::string_view> matched_words(query.plus_words.size());

    auto pred = [this, document_id](const auto& word) {return word_freqs_in_document_.at(document_id).count(word); };

    if (std::any_of(execution::par, query.minus_words.begin(), query.minus_words.end(), pred)) {
        matched_words.clear();
        return { matched_words, documents_.at(document_id).status };
    }
       
    auto end_copy = std::copy_if(query.plus_words.begin(), query.plus_words.end(), matched_words.begin(), pred);
    matched_words.erase(end_copy, matched_words.end());
    std::sort(matched_words.begin(), matched_words.end());
    auto last = std::unique(matched_words.begin(), matched_words.end());
    matched_words.erase(last, matched_words.end());


    return { matched_words, documents_.at(document_id).status };
}



bool SearchServer::IsStopWord(const std::string_view& word) const
{
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string_view& word)
{
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}

vector<string_view> SearchServer::SplitIntoWordsNoStop(string_view text) const {
    vector<string_view> words;
    for (const string_view word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw invalid_argument("Word is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
         }
    }

    return words;
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings)
{
    if (ratings.empty()) {
        return 0;
    }
    return accumulate(ratings.begin(), ratings.end(), 0) / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(const string_view& text) const
{
    if (text.empty()) {
        throw invalid_argument("Query word is empty"s);
    }
    string_view word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word.remove_prefix(1);
    }
    if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
        throw invalid_argument("Query word is invalid");
    }

    return { word, is_minus, IsStopWord(word) };
}

SearchServer::Query SearchServer::ParseQuery(const string_view& text, bool is_sort) const
{
    Query result;
    for (const string_view& word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            }
            else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }
    if (is_sort) {
        std::sort(result.plus_words.begin(), result.plus_words.end());
        auto last = std::unique(result.plus_words.begin(), result.plus_words.end());
        result.plus_words.erase(last, result.plus_words.end());

        std::sort(result.minus_words.begin(), result.minus_words.end());
        last = std::unique(result.minus_words.begin(), result.minus_words.end());
        result.minus_words.erase(last, result.minus_words.end());

    }
    return result;
}

void AddDocument(SearchServer& search_server, const int id, const string& document, DocumentStatus status, const vector<int>& ratings) {
    search_server.AddDocument(id, document, status, ratings);

}

inline std::ostream& operator << (std::ostream& os, const Document& doc) {
    return os << "{ document_id = " << doc.id << ", relevance = " << doc.relevance << ", rating = " << doc.rating << " }";
}

void FindTopDocuments(const SearchServer& search_server, const std::string_view& raw_query)
{
    LOG_DURATION_STREAM("FindTopDocuments time", std::cout);
    auto documents = search_server.FindTopDocuments(raw_query);
    for (const Document& document : documents)
        std::cout << document << std::endl;
}


void MatchDocuments(const SearchServer& search_server, const std::string_view& raw_query)
{
    LOG_DURATION_STREAM("Operation time", std::cout);
    for (int document_id = 0; document_id < search_server.GetDocumentCount(); ++document_id) {
        const auto [words, status] =
            search_server.MatchDocument(raw_query, document_id);
        std::cout << "{ document_id = "s << document_id << ", status = "s << static_cast<int>(status) << ", words = ";
        for (std::string_view word : words)
            std::cout << word << " ";
        std::cout << "}\n";


    }
}
