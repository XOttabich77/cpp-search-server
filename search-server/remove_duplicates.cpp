#include "remove_duplicates.h"
#include "log_duration.h"


using namespace std;

const set<string_view> GetKeyFromMap(const map<string_view, double>& a) {
    set<string_view> b;
    for (auto el:a)
    {
        b.insert(el.first);
    }
    return b;
}

void RemoveDuplicates(SearchServer& search_server) {
    vector<int> to_delete;
    set <set<string_view>> temp_words;

    //sort(search_server.begin(), search_server.end());   
    for (const int document_id : search_server) {
        auto el = temp_words.insert(GetKeyFromMap(search_server.GetWordFrequencies(document_id)));
        if (!el.second) {
            cout << "Found duplicate document id " << document_id << endl;
            to_delete.push_back(document_id);
        }
    }
    for (auto id : to_delete) {
        search_server.RemoveDocument(id);
    }
}
