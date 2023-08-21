#pragma once
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <execution>
#include <string_view>
#include <deque>

#include "document.h"
#include "string_processing.h"
#include "log_duration.h"
#include "concurrent_map.h"

const int MAX_RESULT_DOCUMENT_COUNT = 5;
using StringStatus = std::tuple<std::vector<std::string_view>, DocumentStatus>;

class SearchServer {
public:
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words);
    explicit SearchServer(const std::string& stop_words_text);
   
    void AddDocument(int document_id, const std::string& document, DocumentStatus status,
        const std::vector<int>& ratings);
    //1
    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string_view& raw_query, DocumentPredicate document_predicate) const;
    //2
    std::vector<Document> FindTopDocuments(const std::string_view& raw_query, DocumentStatus status) const;
    //3
    std::vector<Document> FindTopDocuments(const std::string_view& raw_query) const;

    //4 main 
    template <class ExecutionPolicy, typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query, DocumentPredicate document_predicate) const;
    //5
    template <class ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query, DocumentStatus status) const;
    //6
    template <class ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query) const;
   
    int GetDocumentCount() const;
  
    std::set<int>::iterator begin();
    std::set<int>::iterator end();

    const std::map<std::string_view, double>& GetWordFrequencies(int document_id) const;

    void RemoveDocument(int document_id);
    template< class ExecutionPolicy>
    void RemoveDocument(ExecutionPolicy&& policy,int document_id);

    StringStatus MatchDocument(const std::string_view& raw_query,int document_id) const;
    StringStatus MatchDocument(const std::execution::sequenced_policy&, const std::string_view& raw_query, int document_id) const;
    StringStatus MatchDocument(const std::execution::parallel_policy&, const std::string_view& raw_query, int document_id) const;
   

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    const std::set<std::string, std::less<>> stop_words_;
    std::deque<std::string> storage_;
    
    std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
    std::map<int, std::map<std::string_view, double>> word_freqs_in_document_;

    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_; 

    bool IsStopWord(const std::string_view& word) const;
    static bool IsValidWord(const std::string_view& word);
    std::vector<std::string_view> SplitIntoWordsNoStop(std::string_view text) const;
    static int ComputeAverageRating(const std::vector<int>& ratings);

    struct QueryWord {
        std::string_view data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(const std::string_view& text) const;

    struct Query {
        std::vector<std::string_view> plus_words;
        std::vector<std::string_view> minus_words;
    };
   
    Query ParseQuery(const std::string_view& text, bool is_sort) const;
    
    double ComputeWordInverseDocumentFreq(const std::string_view& word) const {
        return std::log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }
    
   
    template <class ExecutionPolicy, typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(ExecutionPolicy&& policy, const Query& query,DocumentPredicate document_predicate) const;
 
};

template<typename StringContainer>
inline SearchServer::SearchServer(const StringContainer& stop_words)
    : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  
{
    if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
        throw std::invalid_argument("Some of stop words are invalid");
    }
}


//1
template<typename DocumentPredicate>
inline std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query, DocumentPredicate document_predicate) const
{
     return FindTopDocuments(std::execution::seq, raw_query, document_predicate);
}

//5
template <class ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query, DocumentStatus status) const
{
    return FindTopDocuments(policy,
        raw_query, [status]([[maybe_unused]] int document_id, DocumentStatus document_status, [[maybe_unused]] int rating) {
            return document_status == status;
        });
}
//6
template <class ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query) const
{
    return FindTopDocuments(policy, raw_query, DocumentStatus::ACTUAL);
}

#define ERROR_RATE 1e-6

// 4 main
template<class ExecutionPolicy, typename DocumentPredicate>
inline std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query, DocumentPredicate document_predicate) const
{
    const auto query = ParseQuery(raw_query, true); 
    auto matched_documents = FindAllDocuments(policy , query, document_predicate); 
    sort(std::execution::par, matched_documents.begin(), matched_documents.end(),
        [](const Document& lhs, const Document& rhs) {
            if (std::abs(lhs.relevance - rhs.relevance) < ERROR_RATE) {
                return lhs.rating > rhs.rating;
            }
            else {
                return lhs.relevance > rhs.relevance;
            }
        });

    
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_documents;    
}

template<class ExecutionPolicy>
inline void SearchServer::RemoveDocument(ExecutionPolicy&& policy, int document_id)
{
    if (documents_.count(document_id)) {
        std::vector<std::string> words(word_freqs_in_document_[document_id].size());
        std::transform(word_freqs_in_document_[document_id].begin(), word_freqs_in_document_[document_id].end(), words.begin(), [](auto& word) {return word.first; });
        std::for_each(policy, words.begin(), words.end(), [this, document_id](std::string& word) {
              word_to_document_freqs_.at(word).erase(document_id);
            });

        documents_.erase(document_id);
        word_freqs_in_document_.erase(document_id);
        document_ids_.erase(document_id);
    }
}


template <class ExecutionPolicy, typename DocumentPredicate>
inline std::vector<Document> SearchServer::FindAllDocuments(ExecutionPolicy&& policy, const Query& query, DocumentPredicate document_predicate) const
{

    static size_t core = std::thread::hardware_concurrency();
    ConcurrentMap<int, double> document_to_relevance(core);


    auto pred_plus = [&document_to_relevance, &document_predicate, this](const auto& word) {
        if (word_to_document_freqs_.count(word) != 0) {
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id].ref_to_value += term_freq * inverse_document_freq;
                }
            }
        }
    };

  for_each(policy, query.plus_words.begin(), query.plus_words.end(), pred_plus);
       
  auto pred_minus = [&document_to_relevance, this](const auto& word) {
      if (word_to_document_freqs_.count(word) != 0) {
          for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
              document_to_relevance.erase(document_id);
          }
      }
  };
  for_each(policy, query.minus_words.begin(), query.minus_words.end(), pred_minus);
  
    
    std::vector<Document> matched_documents;
    for (const auto& [document_id, relevance] : document_to_relevance.BuildOrdinaryMap()) {
        matched_documents.push_back(
            { document_id, relevance, documents_.at(document_id).rating });
    }
    return matched_documents;
}



void MatchDocuments(const SearchServer& search_server, const std::string_view& raw_query);
void AddDocument(SearchServer& search_server, const int id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);
void FindTopDocuments(const SearchServer& search_server, const std::string_view& raw_query);