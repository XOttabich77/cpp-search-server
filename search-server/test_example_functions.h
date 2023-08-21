#pragma once
#include "search_server.h"
#include "document.h"
#include "paginator.h"
#include "request_queue.h"
#include "string_processing.h"
//#include "remove_duplicates.h"
#include "process_queries.h"
#include <execution>


void TestHelloWord();
void TestTotalEmptyRequests();
void TestPaginateMy();
void TestErrorSerchServer();
void TestFindRequest();
void TestProcess_Queries();
void TestProcess_Queries_Join();
void TestDeletePar();
void TestDeleteParBenchmark();
void TestMatchDocum();
void TestMatchDocumentBenchmark();

void TestFindTopDocuments();


