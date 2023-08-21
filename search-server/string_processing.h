#pragma once
#include <string>
#include <vector>
#include <set>
#include <iostream>


std::vector<std::string_view> SplitIntoWords(std::string_view text);


template <typename StringContainer>
std::set<std::string, std::less<>> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string, std::less<>> non_empty_strings;
    for (const std::string_view& s : strings) {
    std::string str{ s };
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}