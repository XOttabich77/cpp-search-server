#pragma once
#include <vector>
#include <string>
#include <list>
#include "search_server.h"


std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);

std::vector<std::vector<Document>> ProcessQueriesSTD(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);

std::list<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);