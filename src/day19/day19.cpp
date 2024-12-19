#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <unordered_map>

using list_t = std::vector<std::string>;
using cache_t = std::unordered_map<std::string, size_t>;

struct towels_t {
    list_t patterns;
    list_t designs;
};

towels_t load_input(const std::string& file){
    towels_t ret;
    std::ifstream fs(file);
    std::string line;
    std::string token;
    std::getline(fs, line);
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    std::istringstream iss(line);
    while (std::getline(iss, token, ',')) {
        ret.patterns.push_back(token);
    }
    std::getline(fs, line);
    while (std::getline(fs, line)) {
        ret.designs.push_back(line);
    }
    return ret;
}

size_t match_pattern(const std::string& towel, const list_t& patterns, cache_t& cache)
{
    if(cache.count(towel)) {
        return cache[towel]; // already calculated this
    }

    size_t count = 0;
    for(auto& pattern : patterns) {
        if(towel.find(pattern) == 0) {
            std::string sub_towel = towel.substr(pattern.size());
            if(sub_towel.empty()) { 
                count++; // reached end of towel design, meaning we have a possible way to make design, so increment count
            }else{
                count += match_pattern(sub_towel, patterns, cache); // continue searching...
            }
        }
    }

    cache[towel] = count; // cache this towel/sub_towel count to prevent recalculation 
    return count;
}

auto process(const towels_t& towels)
{
    size_t valid = 0;
    size_t sum = 0;
    for(auto& towel : towels.designs){
        cache_t cache;
        size_t count = match_pattern(towel, towels.patterns, cache);
        valid += count > 0;
        sum += count;
    }

    return std::make_pair(valid, sum);
}

void main()
{
    auto test_values = load_input("../src/day19/test_input.txt");
    auto actual_values = load_input("../src/day19/input.txt");

    auto [part1_test, part2_test] = process(test_values);
    auto [part1, part2] = process(actual_values);

    std::cout << "part1: " << part1_test << std::endl;
    std::cout << "part1: " << part1 << std::endl;

    std::cout << "part2: " << part2_test << std::endl;
    std::cout << "part2: " << part2 << std::endl;
}