#include "test_example_function.h"

using namespace std;

void TestHelloWord()
{
	std::cout << "hello test" << std::endl;
}

void TestTotalEmptyRequests()
{
    cout << "Test Empty" << endl;
    SearchServer search_server("and in at"s);
    RequestQueue request_queue(search_server);
    search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, { 1, 1, 1 });
    // 1439 запросов с нулевым результатом
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request"s);
    }
    // все еще 1439 запросов с нулевым результатом
    request_queue.AddFindRequest("curly dog"s);
    // новые сутки, первый запрос удален, 1438 запросов с нулевым результатом
    request_queue.AddFindRequest("big collar"s);
    // первый запрос удален, 1437 запросов с нулевым результатом
    request_queue.AddFindRequest("sparrow"s);
    std::cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
}


template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}


ostream& operator << (ostream& os, const Document& doc) {
    return os << "{ document_id = "s << doc.id << ", relevance = "s << doc.relevance << ", rating = "s << doc.rating << " }"s;
}


template <typename Iterator>
ostream& operator << (ostream& os, IteratorRange <Iterator> it) {
    for (auto docum = it.begin(); docum != it.end(); docum++) {
        os << *docum;
    }
    return os;
}

void TestPaginate()
{
    cout << "Test Paginate" << endl;
    SearchServer search_server("and with"s);
    search_server.AddDocument(1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "funny pet with curly hair"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    search_server.AddDocument(3, "big cat nasty hair"s, DocumentStatus::ACTUAL, { 1, 2, 8 });
    search_server.AddDocument(4, "big dog cat Vladislav"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
    search_server.AddDocument(5, "big dog hamster Borya"s, DocumentStatus::ACTUAL, { 1, 1, 1 });
    const auto search_results = search_server.FindTopDocuments("curly dog"s);
    int page_size = 2;
    const auto pages = Paginate(search_results, page_size);
    // ¬ыводим найденные документы по страницам
    for (auto page = pages.begin(); page != pages.end(); ++page) {
        cout << *page << endl;
        cout << "Page break"s << endl;
    }
}

void TestErrorSerchServer()
{
    try {
        cout << "Test Error Serch Server" << endl;
        SearchServer search_server("и в на"s);
        search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
        search_server.AddDocument(10, "пушистый пЄс и модный ошейник"s, DocumentStatus::ACTUAL, { 1, 2 });
        search_server.AddDocument(-1, "пушистый пЄс и модный ошейник"s, DocumentStatus::ACTUAL, {1, 2});
        search_server.AddDocument(3, "большой пЄс скво\x12рец"s, DocumentStatus::ACTUAL, {1, 3, 2});

        const auto documents = search_server.FindTopDocuments("--пушистый"s);
        for (const Document& document : documents) {
           cout <<document;
        }
    }
    catch (const invalid_argument& err) {
        cerr << "\033[1;31mArgument Error: "s << err.what() <<"\033[0m" << endl;
    }
    catch (const out_of_range& err) {
        cerr << "\033[1;31mOut Of range Error: "s << err.what() << "\033[0m"  << endl;
    }
}
