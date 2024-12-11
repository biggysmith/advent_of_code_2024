#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>

using stones_t = std::vector<size_t>;

stones_t load_input(const std::string& file){
    stones_t ret;
    std::ifstream fs(file);
    std::string line;
    std::getline(fs, line);
    size_t number;
    std::istringstream line_stream(line);
    while (line_stream >> number) {
        ret.push_back(number);
    }
    return ret;
}

size_t num_digits(size_t number) {
    size_t digits = 0;
    while (number > 0) {
        number /= 10;
        ++digits;
    }
    return digits;
}

bool even_digits(size_t number) {
    return (num_digits(number) % 2 == 0);
}

auto split(size_t number) {
    size_t half_digits = num_digits(number) / 2;
    size_t divisor = 1;
    for (int i=0; i<half_digits; ++i) {
        divisor *= 10;
    }
    size_t left = number / divisor;
    size_t right = number % divisor;
    return std::make_pair(left, right);
}

struct hasher {
    std::size_t operator()(const std::tuple<size_t,size_t>& t) const {
        return std::hash<size_t>()(std::get<0>(t)) ^ (std::hash<size_t>()(std::get<1>(t)) << 1);
    }
};

using cache_t = std::unordered_map<std::tuple<size_t,size_t>, size_t, hasher>;

size_t blink(size_t stone, cache_t& cache, int depth){
    if(depth == 0){
        return 1;
    }

    if(cache.count({stone,depth})){
        return cache[{stone,depth}];
    }
    
    if(stone == 0){
        return (cache[{stone,depth}] = blink(1, cache, depth-1));
    }else if(even_digits(stone)){
        auto [left, right] = split(stone);
        return (cache[{stone,depth}] = blink(left, cache, depth-1) + blink(right, cache, depth-1));
    }else{
        return (cache[{stone,depth}] = blink(stone*2024, cache, depth-1));
    }
}

size_t process(const stones_t& stones, int depth)
{
    cache_t cache;

    size_t sum = 0;
    for(size_t stone : stones){
        sum += blink(stone, cache, depth);
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day11/test_input.txt");
    auto actual_values = load_input("../src/day11/input.txt");

    std::cout << "part1: " << process(test_values, 25) << std::endl;
    std::cout << "part1: " << process(actual_values, 25) << std::endl;

    std::cout << "part2: " << process(test_values, 75) << std::endl;
    std::cout << "part2: " << process(actual_values, 75) << std::endl;
}