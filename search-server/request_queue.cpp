#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) 
    : search_server_{ search_server },
    counter_(0),
    empty_cuonter_(0)
{
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status)
{
    auto documents = search_server_.FindTopDocuments(raw_query, status);
    AddResultRequest(static_cast<int>(documents.size()));
    return documents;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query)
{
    auto documents = search_server_.FindTopDocuments(raw_query);
    AddResultRequest(static_cast<int>(documents.size()));
    return documents;
}

int RequestQueue::GetNoResultRequests() const
{
    return empty_cuonter_;
}

void RequestQueue::AddResultRequest(int num)
{
    ++counter_;
    while (!requests_.empty() && min_in_day_ <= counter_ - requests_.front().time) {
        if (0 == requests_.front().result) {
            --empty_cuonter_;
        }
        requests_.pop_front();
    }
    requests_.push_back({ counter_, num });
    if (num == 0) {
        ++empty_cuonter_;
    }
}
