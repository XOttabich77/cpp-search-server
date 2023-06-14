#include "remove_duplicates.h"
#include "log_duration.h"


using namespace std;


struct Pair_First_Equal {
    template <typename Pair>
    bool operator() (Pair const& lhs, Pair const& rhs) const {
    return lhs.first == rhs.first;
    }
};

template <typename Map>
bool KeyCompare(Map const& lhs, Map const& rhs) {
    return lhs.size() == rhs.size() && 
            equal(lhs.begin(), lhs.end(),rhs.begin(),Pair_First_Equal()); 
}



void RemoveDuplicates(SearchServer& search_server)
{
    vector<int> to_delete;
    {
       //  LOG_DURATION_STREAM("Operation time find", std::cout);
        sort(search_server.begin(), search_server.end());
        for (auto it = search_server.begin(); it < search_server.end() - 1; ++it) {
            if (count(to_delete.begin(), to_delete.end(), *it)) {
                continue;
            }
            auto a1 = search_server.GetWordFrequencies(*it);
            for (auto it2 = it + 1; it2 < search_server.end(); ++it2) {
                if (count(to_delete.begin(), to_delete.end(), *it2)) {
                    continue;
                }
                {
                    auto a2 = search_server.GetWordFrequencies(*it2);
                    if (KeyCompare(a1, a2)) {
                        cout << "Found duplicate document id " << *it2 << endl;
                        to_delete.push_back(*it2);
                    }
                }
            }
        }
    }
    { 
       // LOG_DURATION_STREAM("Operation time delete", std::cout);
        for (auto i : to_delete) {
            search_server.RemoveDocument(i);
        }
    }
}
