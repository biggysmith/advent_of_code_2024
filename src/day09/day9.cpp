#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <deque>

struct range_t{
    size_t size() const { return end - begin; }
    bool empty() const { return end == begin; }

    size_t id, begin, end;
};

std::vector<int> load_input(const std::string& file){
    std::vector<int> ret;
    std::ifstream fs(file);
    std::string line;
    std::getline(fs, line);
    for(auto c : line){
        ret.push_back(c-'0');
    }
    return ret;
}

size_t part1(const std::vector<int>& disk_map)
{
    std::vector<int> blocks;
    int free_space = 0;

    for(size_t i=0, id=0; i<disk_map.size(); i+=2, ++id) { // make full sequence
        blocks.insert(blocks.end(), disk_map[i], (int)id); 
        if(i+1 < disk_map.size()) {
            blocks.insert(blocks.end(), disk_map[i+1], -1);
            free_space += disk_map[i+1];
        }
    }

    int moved = 0;
    for(size_t i=0; moved<free_space && i<blocks.size(); ++i) { // swap end with free blocks
        if(blocks[i] == -1) {
            while(blocks.back() == -1) {
                blocks.pop_back();
            }
            if(i >= blocks.size()) {
                break;
            }
            std::swap(blocks[i], blocks.back());
            blocks.pop_back();
            ++moved;
        }
    }

    size_t sum = 0;
    for (size_t i=0; i<blocks.size(); ++i) {
        sum += i * blocks[i];
    }
    return sum;
}

size_t part2(const std::vector<int>& disk_map)
{
    std::vector<range_t> used;
    std::deque<range_t> free;

    for(size_t i=0, id=0, pos=0; i<disk_map.size(); i+=2, ++id) { // make full used + free sequence ranges
        size_t used_size = disk_map[i];
        size_t free_size = (i+1 < disk_map.size()) ? disk_map[i+1] : 0;

        used.push_back({id, pos, pos + used_size});
        pos += used_size;

        if(free_size > 0) {
            free.push_back({UINT64_MAX, pos, pos + free_size});
            pos += free_size;
        }
    }

    for(auto used_it = used.rbegin(); used_it != used.rend(); ++used_it) { // shift used ranges into suitable free ranges
        size_t used_size = used_it->size();

        for (auto it = free.begin(); it != free.end() && it->begin < used_it->begin;) {
            if(it->size() >= used_size) {
                *used_it = { used_it->id, it->begin, it->begin + used_size };
                it->begin += used_size;
                it->empty() ? (it = free.erase(it)) : ++it;
                break;
            } else {
                ++it;
            }
        }
    }

    size_t sum = 0;
    for(auto& range : used) {
        for(size_t i=range.begin; i<range.end; ++i) {
            sum += range.id * i;
        }
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day09/test_input.txt");
    auto actual_values = load_input("../src/day09/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}