#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <set>
#include <array>
#include <numeric>
#include <unordered_set>

#include <timer.hpp>
#include <optional>
#include <map>
#include <unordered_map>

std::vector<size_t> load_input(const std::string& file){
    std::vector<size_t> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(stoull(line));
    }
    return ret;
}

size_t prune(size_t num){ return num % 16777216; }
size_t mix(size_t num_a, size_t num_b){ return num_a ^ num_b; }
int ones(size_t num){ return (int)(num % 10); }

size_t next_secret_num(size_t secret){
    secret = prune(mix(secret << 6, secret));
    secret = prune(mix(secret >> 5, secret));
    secret = prune(mix(secret << 11, secret));
    return secret;
}

size_t part1(const std::vector<size_t>& numbers)
{
    size_t sum = 0;
    for(size_t secret : numbers){
        for(int i=0; i<2000; ++i){
            secret = next_secret_num(secret);
        }
        sum += secret;
    }
    return sum;
}



std::vector<int> find_occurrences(const std::vector<int>& bigger_seq, const std::array<int,4>& seq) {
    std::vector<int> occurrences;

    auto it = bigger_seq.begin();
    while ((it = std::search(it, bigger_seq.end(), seq.begin(), seq.end())) != bigger_seq.end()) {
        occurrences.push_back((int)(it - bigger_seq.begin()));  // Store the starting index of the match
        ++it;  // Move to the next position to continue searching
    }

    return occurrences;
}

std::optional<int> find_first_occurrence(const std::vector<int>& bigger_seq, const std::array<int,4>& seq) {
    if (seq.empty() || seq.size() > bigger_seq.size()) {
        return std::nullopt; // Return no value if seq is invalid
    }

    // Use std::search to find the first occurrence
    auto it = std::search(bigger_seq.begin(), bigger_seq.end(), seq.begin(), seq.end());
    
    if (it != bigger_seq.end()) {
        return (int)std::distance(bigger_seq.begin(), it); // Return the index of the first occurrence
    }

    return std::nullopt; // No match found
}

using seq_t = std::array<int, 4>;

struct hasher {
    std::size_t operator()(const seq_t& arr) const {
        return (arr[3]+9)*(19*19*19) + (arr[2]+9)*(19*19) + (arr[1]+9)*19 + (arr[0]+9);
    }
};

using set_t = std::unordered_set<seq_t, hasher>;
//using set_t = std::set<seq_t>;

void find_change_set(const std::vector<int>& change_list, set_t& change_set){
    for(int i=0; i<change_list.size()-4; ++i){
        seq_t arr { change_list[i+0], change_list[i+1], change_list[i+2], change_list[i+3] };
        //if(!change_set.count(arr)){
            change_set.insert(arr);
        //}
    }
}

int part2(const std::vector<size_t>& numbers)
{
    scoped_timer t;

    std::cout << std::endl;

    std::vector<std::vector<int>> price_list;
    std::vector<std::vector<int>> change_list;
    for(size_t secret : numbers){
        price_list.push_back(std::vector<int>());
        change_list.push_back(std::vector<int>());
        //change_list.back().push_back(secret);
        for(int i=0; i<2000; ++i){
            size_t prev_secret = secret;
            secret = next_secret_num(secret);
            change_list.back().push_back(ones(secret)-ones(prev_secret));
            price_list.back().push_back(ones(secret));
        }
    }

    set_t change_set;
    for(int i=0; i<numbers.size(); ++i){
        find_change_set(change_list[i], change_set);
    }

    std::cout << change_set.size() << std::endl;

    int max_bananas = INT_MIN;
    for(auto& change4 : change_set){
        std::vector<int> prices(numbers.size(), INT_MIN);
        for(int i=0; i<numbers.size(); ++i){
            //auto occurances = find_occurrences(change_list[i], change4);
            auto occurance = find_first_occurrence(change_list[i], change4);
            /*for(auto occurance : occurances){
                if(price_list[i][occurance+4-1] > prices[i]){
                    prices[i] = price_list[i][occurance+4-1];
                    break;
                }
            }*/
            if(occurance){
                prices[i] = std::max(prices[i], price_list[i][*occurance+4-1]);
            }
        }

        int bananas = std::accumulate(prices.begin(), prices.end(), 0, [](int acc, int price){
            return acc + (price==INT_MIN ? 0 : price);
        });

        if(bananas > max_bananas){
            std::cout << change4[0] << "," << change4[1] << "," << change4[2] << "," << change4[3] << ": " << bananas << std::endl;
            max_bananas = std::max(max_bananas, bananas);
        }
    }

    return max_bananas;
}

using change_set_t = std::unordered_map<std::array<int,4>, int, hasher>;

void find_change_set2(const std::vector<int>& change_list, const std::vector<int>& price_list, change_set_t& change_set, set_t& global_change_set){
    for(int i=0; i<change_list.size()-4; ++i){
        seq_t arr { change_list[i+0], change_list[i+1], change_list[i+2], change_list[i+3] };
        if(!change_set.count(arr)){
            change_set[arr] = price_list[i+3];
        }
        //if(!global_change_set.count(arr)){
            global_change_set.insert(arr);
        //}
    }
}

int part3(const std::vector<size_t>& numbers)
{
    scoped_timer t;

    std::cout << std::endl;

    std::vector<std::vector<int>> price_list(numbers.size());
    std::vector<std::vector<int>> change_list(numbers.size());

    for(int i = 0; i<numbers.size(); ++i) {
        price_list[i].resize(2000);
        change_list[i].resize(2000);
        size_t secret = numbers[i];
        for(int j=0; j<2000; ++j) {
            size_t prev_secret = secret;
            secret = next_secret_num(secret);
            price_list[i][j] = ones(secret);
            change_list[i][j] = ones(secret) - ones(prev_secret);
        }
    }

    set_t change_set;
    std::vector<change_set_t> local_change_sets(numbers.size());
    {
        scoped_timer t2;
        for(int i = 0; i<numbers.size(); ++i) {
            //scoped_timer t2(e_microseconds);
            find_change_set2(change_list[i], price_list[i], local_change_sets[i], change_set);
        }
    }

    //std::cout << change_set.size() << std::endl;

    int max_bananas = INT_MIN;
    for(auto& change4 : change_set){
        std::vector<int> prices(numbers.size(), INT_MIN);
        /*for(int i=0; i<numbers.size(); ++i){
            auto occurance = find_first_occurrence(change_list[i], change4);
            if(occurance){
                prices[i] = std::max(prices[i], price_list[i][*occurance+4-1]);
            }
        }*/

        for(int i=0; i<numbers.size(); ++i){
            if(local_change_sets[i].count({change4})){
                prices[i] = local_change_sets[i][change4];
            }
        }

        int bananas = std::accumulate(prices.begin(), prices.end(), 0, [](int acc, int price){
            return acc + (price==INT_MIN ? 0 : price);
        });

        if(bananas > max_bananas){
            //std::cout << change4[0] << "," << change4[1] << "," << change4[2] << "," << change4[3] << ": " << bananas << std::endl;
            max_bananas = std::max(max_bananas, bananas);
        }
    }

    return max_bananas;
}

void main()
{
    auto test_values0 = load_input("../src/day22/test_input0.txt");
    auto test_values1 = load_input("../src/day22/test_input1.txt");
    auto actual_values = load_input("../src/day22/input.txt");

    std::cout << "part1: " << part1(test_values0) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part3(test_values1) << std::endl;
    std::cout << "part2: " << part3(actual_values) << std::endl;
}