#include "test_example_function.h"
#include "log_duration.h"
#include "document.h"

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
    // 1439 �������� � ������� �����������
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request"s);
    }
    // ��� ��� 1439 �������� � ������� �����������
    request_queue.AddFindRequest("curly dog"s);
    // ����� �����, ������ ������ ������, 1438 �������� � ������� �����������
    request_queue.AddFindRequest("big collar"s);
    // ������ ������ ������, 1437 �������� � ������� �����������
    request_queue.AddFindRequest("sparrow"s);
    std::cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
}



void TestPaginateMy()
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
    // ������� ��������� ��������� �� ���������
    for (auto page = pages.begin(); page != pages.end(); ++page) {
        cout << *page << endl;
        cout << "Page break"s << endl;
    }
}

void TestFindRequest() {
    SearchServer search_server("and in at"s);
    RequestQueue request_queue(search_server);
    search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, { 1, 1, 1 });
    // 1439 �������� � ������� �����������
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request"s);
    }
    // ��� ��� 1439 �������� � ������� �����������
    request_queue.AddFindRequest("curly dog"s);
    // ����� �����, ������ ������ ������, 1438 �������� � ������� �����������
    request_queue.AddFindRequest("big collar"s);
    // ������ ������ ������, 1437 �������� � ������� �����������
    request_queue.AddFindRequest("sparrow"s);
    cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
}

void TestErrorSerchServer()
{
    try {
        cout << "Test Error Serch Server" << endl;
        SearchServer search_server("� � ��"s);
        search_server.AddDocument(1, "�������� ��� �������� �����"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
        search_server.AddDocument(10, "�������� �� � ������ �������"s, DocumentStatus::ACTUAL, { 1, 2 });
        search_server.AddDocument(-1, "�������� �� � ������ �������"s, DocumentStatus::ACTUAL, {1, 2});
        search_server.AddDocument(3, "������� �� ����\x12���"s, DocumentStatus::ACTUAL, {1, 3, 2});

        const auto documents = search_server.FindTopDocuments("--��������"s);
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


void TestDelDublicate()
{
    SearchServer search_server("and with"s);

    AddDocument(search_server, 1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    AddDocument(search_server, 2, "funny pet with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });

    // �������� ��������� 2, ����� �����
    AddDocument(search_server, 3, "funny pet with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });

    // ������� ������ � ����-������, ������� ����������
    AddDocument(search_server, 4, "funny pet and curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });

    // ��������� ���� ����� ��, ������� ���������� ��������� 1
    AddDocument(search_server, 5, "funny funny pet and nasty nasty rat"s, DocumentStatus::ACTUAL, { 1, 2 });

    // ���������� ����� �����, ���������� �� ��������
    AddDocument(search_server, 6, "funny pet and not very nasty rat"s, DocumentStatus::ACTUAL, { 1, 2 });

    // ��������� ���� ����� ��, ��� � id 6, �������� �� ������ �������, ������� ����������
    AddDocument(search_server, 7, "very nasty rat and not very funny pet"s, DocumentStatus::ACTUAL, { 1, 2 });

    // ���� �� ��� �����, �� �������� ����������
    AddDocument(search_server, 8, "pet with rat and rat and rat"s, DocumentStatus::ACTUAL, { 1, 2 });

    // ����� �� ������ ����������, �� �������� ����������
    AddDocument(search_server, 9, "nasty rat with curly hair"s, DocumentStatus::ACTUAL, { 1, 2 });
/*
    for (int i = 50999; i > 100; --i) {
        AddDocument(search_server, i, "very nasty rat and not very funny pet ttyytt"s, DocumentStatus::ACTUAL, { 1, 2 });
    }
    */
    cout << "Before duplicates removed: "s << search_server.GetDocumentCount() << endl;
    
    RemoveDuplicates(search_server);
    cout << "After duplicates removed: "s << search_server.GetDocumentCount() << endl;
}


