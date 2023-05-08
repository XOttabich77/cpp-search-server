#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
      const vector<string> words = SplitIntoWordsNoStop(document);
      ++document_count_;
      double tf_for_word=1.0/words.size();
      for(string word:words){
          documents_[word][document_id]+=tf_for_word;
      }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    
    map<string, map<int,double>> documents_;
    int document_count_ = 0;
    set<string> stop_words_;
    struct Query {
        set<string> p_word;
        set<string> m_word;
    };

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (string& word : SplitIntoWordsNoStop(text)) {
            if(word[0]=='-'){
                word.erase(0,1);
                query_words.m_word.insert(word);
            }
            else {
               query_words.p_word.insert(word); 
            }
        }
        return query_words;
    }
    
    double Idf_Word (const string& word) const{
        return log(document_count_*1.0/documents_.at(word).size());
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map <int,double> index_doc;
            
       // ищем по слову есть ли оно в документах
      for(const string& word:query_words.p_word){
        if(documents_.count(word)!=0){
               const double idf_word=Idf_Word(word);   
               for(const auto [id,rl]:documents_.at(word)){
                   index_doc[id]+=rl*idf_word;
               }
            }    
        }
      //  удаляем минус слова
       for(const string& word:query_words.m_word){
           if(documents_.count(word)!=0){
                  for(const auto [id,rl]:documents_.at(word)){
                      index_doc.erase(id);
                  }
           }  
       }
        
    // переносим найденые документы    
       for(const auto& [key,value]:index_doc){
           matched_documents.push_back({key,value});
       }
              
    return matched_documents;
    }

  
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}