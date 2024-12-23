#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <array>
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

using seq_t = std::array<int, 4>;

struct hasher {
    std::size_t operator()(const seq_t& arr) const {
        return (arr[3]+9)*(19*19*19) + (arr[2]+9)*(19*19) + (arr[1]+9)*19 + (arr[0]+9);
    }
};

using seq_price_map_t = std::unordered_map<std::array<int,4>, int, hasher>;

int part2(const std::vector<size_t>& numbers)
{
    int max_bananas = INT_MIN;
    seq_t change_seq;
    seq_price_map_t seq_price_max;

    for(size_t number : numbers) {
        seq_price_map_t seq_price_map;

        size_t secret = number;
        for(int i=0; i<2000; ++i) {
            size_t prev_secret = secret;
            secret = next_secret_num(secret);

            int price_i = ones(secret);
            change_seq[i % 4] = ones(secret) - ones(prev_secret);

            if(i >= 3){
                seq_t arr { change_seq[(i-3) % 4], change_seq[(i-2) % 4], change_seq[(i-1) % 4], change_seq[i % 4] };
                if(!seq_price_map.count(arr)){
                    seq_price_map[arr] = price_i;
                }
            }
        }

        for(auto& [seq, price] : seq_price_map){
            seq_price_max[seq] += price;
            max_bananas = std::max(max_bananas, seq_price_max[seq]);
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

    std::cout << "part2: " << part2(test_values1) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}