#include "search_server.h"

using namespace std;

SearchServer::SearchServer(const std::string& stop_words_text)
    : SearchServer(
        SplitIntoWords(stop_words_text))  // Invoke delegating constructor from string container
{
}

void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const vector<int>& ratings)
{       
        if ((document_id < 0) || (documents_.count(document_id) > 0)) {
            throw invalid_argument("Invalid document_id"s);
        }
        const auto words = SplitIntoWordsNoStop(document);

        const double inv_word_count = 1.0 / words.size();
        for (const std::string& word : words) {
         //   word_to_document_freqs_[word][document_id] += inv_word_count;
            word_to_document_freqs1_[document_id][word] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ SearchServer::ComputeAverageRating(ratings), status });
        document_ids_.push_back(document_id);
 }

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const
{
    return FindTopDocuments(
        raw_query, [status]([[maybe_unused]] int document_id, DocumentStatus document_status, [[maybe_unused]] int rating) {
            return document_status == status;
        });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const
{
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const
{
    return static_cast<int>(documents_.size());
}

int SearchServer::GetDocumentId(int index) const
{
    return document_ids_.at(index);
}

vector<int>::iterator SearchServer::begin() 
{
    return document_ids_.begin();
}

vector<int>::iterator SearchServer::end() 
{
    return document_ids_.end();
}

const map<string, double>& SearchServer::GetWordFrequencies(int document_id) const
{
    
    if (word_to_document_freqs1_.count(document_id) == 0) {
    static const map<std::string, double> empty;
            return empty;
    }              
    return word_to_document_freqs1_.at(document_id);;
}

void SearchServer::RemoveDocument(int document_id)
{
    word_to_document_freqs1_.erase(document_id);
    documents_.erase(document_id);
  //  sort(document_ids_.begin(), document_ids_.end());
  //  auto it_v = lower_bound(document_ids_.begin(), document_ids_.end(), document_id);
    auto it_v = find(document_ids_.begin(), document_ids_.end(), document_id);
    document_ids_.erase(it_v);
}

tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const
{
   const auto query = SearchServer::ParseQuery(raw_query);

    vector<string> matched_words;
    if (word_to_document_freqs1_.count(document_id) != 0) {
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs1_.at(document_id).count(word) == 0) {
                continue;
            }
            matched_words.push_back(word);
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs1_.at(document_id).count(word) == 0) {
                continue;
            }
            matched_words.clear();
            break;
        }
    }
    return { matched_words, documents_.at(document_id).status };
}



bool SearchServer::IsStopWord(const std::string& word) const
{
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string& word)
{
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}

vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw invalid_argument("Word "s + word + " is invalid"s);
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
    int rating_sum = 0;
    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(const string& text) const
{
    if (text.empty()) {
        throw invalid_argument("Query word is empty"s);
    }
    string word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word = word.substr(1);
    }
    if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
        throw invalid_argument("Query word "s + text + " is invalid");
    }

    return { word, is_minus, IsStopWord(word) };
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const
{
    Query result;
    for (const string& word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.insert(query_word.data);
            }
            else {
                result.plus_words.insert(query_word.data);
            }
        }
    }
    return result;
}

void AddDocument(SearchServer& search_server, const int id, const string& document, DocumentStatus status, const vector<int>& ratings) {
    search_server.AddDocument(id, document, status, ratings);

}

inline std::ostream& operator << (std::ostream& os, const Document& doc) {
    return os << "{ document_id = " << doc.id << ", relevance = " << doc.relevance << ", rating = " << doc.rating << " }";
}

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query)
{
    LOG_DURATION_STREAM("FindTopDocuments time", std::cout);
    auto documents = search_server.FindTopDocuments(raw_query);
    for (const Document& document : documents)
        std::cout << document << std::endl;
}


void MatchDocuments(const SearchServer& search_server, const std::string& raw_query)
{
    LOG_DURATION_STREAM("Operation time", std::cout);
    for (int document_id = 0; document_id < search_server.GetDocumentCount(); ++document_id) {
        const auto [words, status] =
            search_server.MatchDocument(raw_query, document_id);
        std::cout << "{ document_id = "s << document_id << ", status = "s << static_cast<int>(status) << ", words = ";
        for (std::string word : words)
            std::cout << word << " ";
        std::cout << "}\n";


    }
}
