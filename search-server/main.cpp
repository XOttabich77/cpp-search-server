﻿#include <iostream>

using namespace std;

#include "search_server.h"
#include "paginator.h"
#include "request_queue.h"

#include "test_example_function.h"


int main() {

    TestHelloWord();

    TestTotalEmptyRequests();
    TestPaginate();
    TestErrorSerchServer();
        
    return 0;
}
