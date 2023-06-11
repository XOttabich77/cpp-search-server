#pragma once
#include <vector>
#include <string>
#include <stack>

#include "document.h"
#include "search_server.h"

class RequestQueue {

public:
    explicit RequestQueue(const SearchServer& search_server);
 
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    
    int GetNoResultRequests() const;

private:
    struct QueryResult {
        uint64_t time;
        int result;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
    uint64_t counter_;
    int empty_cuonter_;
    
    void AddResultRequest(int num);
};

template<typename DocumentPredicate>
inline std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate)
{
    auto documents = search_server_.FindTopDocuments(raw_query, document_predicate);
    AddResultRequest(documents.size());
    return documents;
}
