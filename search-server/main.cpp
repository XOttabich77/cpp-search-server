#include <iostream>

using namespace std;

#include "search_server.h"
#include "paginator.h"
#include "request_queue.h"
#include "process_queries.h"

#include "test_example_functions.h"

int main() {
    TestHelloWord();
    TestFindTopDocuments();

   // TestMatchDocum();
   // TestMatchDocumentBenchmark();

    //   TestProcess_Queries();
   //     TestProcess_Queries_Join();
   //     TestDeletePar();
   //     TestDeleteParBenchmark();


   /* TestFindRequest();
    TestPaginateMy();
    TestDelDublicate();
    */
    return 0;
}