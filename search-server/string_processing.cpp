#include "string_processing.h"

using namespace std;

vector<string_view> SplitIntoWords(string_view text)
{
    vector<string_view> words;
    int64_t end_of_str = text.npos;
    while (true) {
        int64_t position = text.find(' ');
        if (position == end_of_str) {
            words.push_back(text.substr(0));
            break;
        }
        else {
            words.push_back(text.substr(0, position));
            text.remove_prefix(position + 1);
        }
    }
    return words;
}
