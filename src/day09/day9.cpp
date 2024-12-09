#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <algorithm>

#undef NDEBUG
#include <assert.h> 

using disk_map_t = std::vector<int>;

struct range_t{
    size_t id;
    size_t begin;
    size_t end;
};

bool operator<(const range_t& a, const range_t& b){ return std::tuple(a.begin, a.end) < std::tuple(b.begin, b.end); }

disk_map_t load_input(const std::string& file){
    disk_map_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        for(auto c : line){
            ret.push_back(c-'0');
        }
    }
    return ret;
}

size_t part1(const disk_map_t& disk_map)
{
    std::vector<int> expanded_blocks;
    int id = 0;
    for(int i=0; i<disk_map.size(); i+=2){
        for(int j=0; j<disk_map[i]; j++){
            expanded_blocks.push_back(id);
        }
        if(i<disk_map.size()-1){
            for(int k=0; k<disk_map[i+1]; k++){
                expanded_blocks.push_back(-1);
            }
        }
        id++;
    }

    std::vector<int> compressed_blocks;

    int j = 0;
    size_t free_space = std::count(expanded_blocks.begin(), expanded_blocks.end(), -1);
    for(int i=0; i<expanded_blocks.size(); ++i){
        if(expanded_blocks[i] == -1){
            while(expanded_blocks.back() == -1){
                expanded_blocks.pop_back();
            }
            if(i >= expanded_blocks.size()){
                break;
            }
            std::swap(expanded_blocks[i], expanded_blocks.back());
            expanded_blocks.pop_back();
            j++;
            if(j >= free_space){
                break;
            }
        }
    }

    size_t sum = 0;
    for(int i=0; i<expanded_blocks.size(); ++i){
        sum += i * expanded_blocks[i];
    }

    return sum;
}

size_t part2(const disk_map_t& disk_map)
{
    std::set<range_t> used_ranges;
    std::set<range_t> free_ranges;

    std::vector<range_t> used;
    std::vector<range_t> free;
    for(size_t i=0, j=0, id=0; i<disk_map.size(); i+=2, id++){
        range_t ur { id, j, j+disk_map[i] };
        used.push_back(ur);
        used_ranges.insert(ur);
        j += disk_map[i];
        if(i<disk_map.size()-1){
            free.push_back({ UINT64_MAX, j, j+disk_map[i+1] });
            free_ranges.insert({ UINT64_MAX, j, j+disk_map[i+1] });
            j += disk_map[i+1];
        }else{
            free.push_back({ UINT64_MAX, j, j });
        }
    }

    std::set<range_t> new_used_ranges = used_ranges;
    
    for(int i=(int)used.size()-1; i>=0; --i){
        auto& us = used[i];
        size_t used_range = us.end - us.begin;

        for(int j=0; j<i; j++){
            size_t free_range = free[j].end - free[j].begin;
            if(free_range > 0 && free_range >= used_range){
                new_used_ranges.erase({ used[i].id, used[i].begin, used[i].end });
                new_used_ranges.insert({ used[i].id, free[j].begin, free[j].begin+used_range });
                free[j].begin = free[j].begin+used_range;
                break;
            }
        }
    }

    size_t sum = 0;
    for(auto& r : new_used_ranges){
        for(size_t j=r.begin; j<r.end; j++){
            sum += r.id * j;
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

    assert(part1(test_values) == 1928);
    assert(part1(actual_values) == 6154342787400);
    assert(part2(test_values) == 2858);
    assert(part2(actual_values) == 6183632723350);
}