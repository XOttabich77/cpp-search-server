#include "process_queries.h"
#include <execution>

//using namespace std;

std::vector<std::vector<Document>> ProcessQueries(const SearchServer& search_server, const std::vector<std::string>& queries)
{
    std::vector<std::vector<Document>> documents_lists(queries.size());
    std::transform(std::execution::par, queries.begin(), queries.end(), documents_lists.begin(), [&search_server](auto &query) {return search_server.FindTopDocuments(query);});
    return documents_lists;
}

std::vector<std::vector<Document>> ProcessQueriesSTD(const SearchServer& search_server, const std::vector<std::string>& queries)
{
    std::vector<std::vector<Document>> documents_lists;
    for (const std::string& query : queries) {
        documents_lists.push_back(search_server.FindTopDocuments(query));
    }
    return documents_lists;
}

std::list<Document> ProcessQueriesJoined(const SearchServer& search_server, const std::vector<std::string>& queries)
{
    std::vector<std::vector<Document>> documents_lists = ProcessQueries(search_server, queries);
    std::list<Document > lists;
    std::for_each(documents_lists.begin(), documents_lists.end(), [&lists](const auto& list_doc)
        {
            for (const auto& doc : list_doc) {
                lists.push_back(doc);                            
            }
        }       );
    return lists;
}

