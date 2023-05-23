#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <numeric>

using namespace std;

/* ���������� ���� ���������� ������ SearchServer ���� */
#include "search_server.h"
/*
   ���������� ���� ���� ���������� ��������
   ASSERT, ASSERT_EQUAL, ASSERT_EQUAL_HINT, ASSERT_HINT � RUN_TEST
*/
#include "framework.h"


// -------- ������ ��������� ������ ��������� ������� ----------
//1 ���������� ����������. ����������� �������� ������ ���������� �� ���������� �������, ������� �������� ����� �� ���������.

void TestAddDocAndFind(){
    const int doc_id = 15;
    const string content = "��������������� ���������� ����� � ������ � ����� ��������������� C++, ������ �������� � ����"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("�����"s);
        ASSERT_EQUAL_HINT(found_docs.size(), 1u,"Add Document and Find it"s);
    }
}

// 2 ��������� ����-����. ����-����� ����������� �� ������ ����������.
// ���� ���������, ��� ��������� ������� ��������� ����-����� ��� ���������� ����������

void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(),
                    "Stop words must be excluded from documents"s);
    }
}
//3 ��������� �����-����. ���������, ���������� �����-����� �� ���������� �������, �� ������ ���������� � ���������� ������.
void TestMinusWord(){
    const int doc_id = 15;
    const string content = "��������������� ���������� ����� ����� � ������ � ����� ��������������� C++, ������ �������� � ����"s;
    const string content1 = "� ����� ������ ������� ����������� �������� ����� ������������� ��������� �������� ������ ������ ���� ������������� ������������� �������� ����� ��� ������ ����� ����� ������ ��� ��������� ������������� ���"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id+1, content1, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("����� -������"s);
        ASSERT_EQUAL_HINT(found_docs.size(), 1u,"Minus Word Test"s);
    }
}

// 4 ������������ ���������� ���������� �������. ��� ���� ������ ���� ���������� ��� ����� �� ���������� �������,
// �������������� � ���������. ���� ���� ������������ ���� �� �� ������ �����-�����,
// ������ ������������ ������ ������ ����.

vector<string> FillDocumentsVector(){
     vector<string> docum ={"���������� aaaa ���������� ����� �� ���������"s,
    "��������� ����-����aaaa ����������� �� ������ ����������"s,
    "��������� ���������� ������� �� ���������� � ���������� ������"s,
    "���������� aaaa ������������� � ������� �������� �������������"s,
    "���������� �������� ���������� ��������������� ������ ���������."s,
    "���������� ����������� �  aaaa ���������, ����������� �������������"s,
    "����� ����������, ������� �������� ������"s,
    "���������� ���������� ������������� ��������� ����������"s};
    return docum;   
}
vector<vector<int>> FillRatingVector(){
const vector<vector<int>> ratings = {{1, 2, 3}, {1,3,5}, {1,2,3,4},
                                    {1,2,3,4,4,4}, {4,5,5,5}, {6,1,2,5,6,4,6},
                                    {7,7,7,7,7,7,7,7,7}, {8,2,8,2}};
return ratings;
}

#define FOR_TEST const auto docum = FillDocumentsVector(); const auto ratings = FillRatingVector();\
                 SearchServer server; int i=0; for(const auto& doc:docum){ \
                 server.AddDocument(i, doc, DocumentStatus::ACTUAL, ratings[i]); ++i; }
                 

void TastMatching(){
    FOR_TEST
    server.AddDocument(11,"jhggjhjhg bbbb jhggjhjhg aaaa YYyyyyy" , DocumentStatus::BANNED, ratings[1]);
    {
    const auto [words,doc_st] = server.MatchDocument("aaaa bbbb"s,11);
    ASSERT_HINT(words.size() == 2,"Test Matching Documents: 1"s);
    ASSERT_EQUAL_HINT(words[0],"aaaa"s, "Test Matching Documents: first word"s);
    ASSERT_EQUAL_HINT(words[1],"bbbb"s, "Test Matching Documents: second word"s);
    }
    {
    const auto [words,doc_st] = server.MatchDocument("aaaa -bbbb"s,11);
    ASSERT_HINT(words.empty(),"Test Matching Documents Minus Word"s);
    }
}
// 5 ���������� ��������� ���������� �� �������������. ������������ ��� ������ ���������� ���������� ������ 
// ���� ������������� � ������� �������� �������������.

void TestRelCountDown(){
    FOR_TEST
    double test_rel=1;
    for(const auto [id,rel,raiting] : server.FindTopDocuments("aaaa"s)){
    ASSERT_HINT(test_rel >= rel,"Test Relivation fail"s);
    test_rel=rel;
    }
}
// 6 ���������� �������� ����������. ������� ������������ ��������� ����� �������� ��������������� ������ ���������.
void TestRatingCalculation(){
    FOR_TEST
    for(const auto [id,rel,raiting] : server.FindTopDocuments("aaaa"s)){
    const int i = (accumulate(ratings[id].begin(),ratings[id].end(),0)/ratings[id].size());
    ASSERT_HINT(raiting == i,"Test Rating crush"s);
    }
}

// 7 ���������� ����������� ������ � �������������� ���������, ����������� �������������.

void TestUserFunction(){
    FOR_TEST
    server.AddDocument(11,"jhggjhjhg bbbb jhggjhjhg aaaa YYyyyyy" , DocumentStatus::BANNED, ratings[1]);
    const auto found_docs = server.FindTopDocuments("aaaa"s, [](int document_id, DocumentStatus status, int rating) { return document_id  == 11 || rating == 3; }) ;
    ASSERT_HINT(found_docs.size() == 2,"Test User function"s);
    ASSERT_HINT(found_docs[0].id == 11 || found_docs[1].id == 11, "Test User function: ID"s);
    ASSERT_HINT(found_docs[0].rating == 3 || found_docs[1].rating == 3, "Test User function: Rating"s);
}

// 8 ����� ����������, ������� �������� ������.
void TestStatusDoc(){
    SearchServer server;
    server.AddDocument(11,"jhggjhjhg bbbb jhggjhjhg aaaa YYyyyyy" , DocumentStatus::BANNED, {1,2,3});
    const auto found_docs = server.FindTopDocuments("aaaa"s,DocumentStatus::BANNED);
    ASSERT_HINT(found_docs.size() == 1,"Test Find Status"s);

}
// 9 ���������� ���������� ������������� ��������� ����������.
void TestRelevanceIsCorrect(){
    FOR_TEST
    const auto found_docs = server.FindTopDocuments("aaaa"s);
    //  double test_rel = found_docs[1].relevance;
    ASSERT_EQUAL_HINT(found_docs[1].relevance, log(8*1.0/3)*1.0/7 ,"Relevance is not correct"s);
    // ASSERT_HINT(abs(test_rel-0.140118)< 1E-6,"Rel is not correct"s);
}
/*
���������� ��� ��������� ������ �����
*/

// ������� TestSearchServer �������� ������ ����� ��� ������� ������
void TestSearchServer() {
    RUN_TEST(TestAddDocAndFind);
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestMinusWord);
    RUN_TEST(TastMatching);
    RUN_TEST(TestRelCountDown);
    RUN_TEST(TestRatingCalculation);
    RUN_TEST(TestUserFunction);
    RUN_TEST(TestStatusDoc);
    RUN_TEST(TestRelevanceIsCorrect);
   
}

// --------- ��������� ��������� ������ ��������� ������� -----------

int main() {
    TestSearchServer();
    // ���� �� ������ ��� ������, ������ ��� ����� ������ �������
    cout << "Search server testing finished"s << endl;
}