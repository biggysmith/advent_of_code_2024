#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>

struct pos_t{
    int x, y;
};
bool operator<(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) < std::tie(b.x, b.y); }

using schematic_t = std::set<pos_t>;
using schematics_t = std::vector<schematic_t>;

schematics_t load_input(const std::string& file){
    schematics_t ret;
    int mode = 0;
    std::ifstream fs(file);
    std::string line;
    int height = 0;
    ret.push_back(schematic_t());
    while (std::getline(fs, line)) {
        if(line.empty()){
            ret.push_back(schematic_t());
            height = 0;
            continue;
        }
        for(int x=0; x<line.size(); ++x){
            if(line[x] == '#') { ret.back().insert({x, height}); }
        }
        height++;
    }
    return ret;
}

int part1(const schematics_t& schematics)
{
    std::vector<schematic_t> keys, locks;

    for(auto& schematic : schematics){
        bool is_lock = schematic.count({0,0});

        if(is_lock){
            locks.push_back(schematic);
        }else{
            keys.push_back(schematic);
        }
    }

    int sum = 0;

    for(auto& key : keys){
        for(auto& lock : locks){

            std::map<pos_t,int> combined;
            for(auto& key_pos : key){
                combined[key_pos]++;
            }
            for(auto& lock_pos : lock){
                combined[lock_pos]++;
            }

            bool fits = std::all_of(combined.begin(), combined.end(), [](auto& c){ return c.second == 1; });
            sum += fits;

        }
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day25/test_input.txt");
    auto actual_values = load_input("../src/day25/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << "Merry Christmas!" << std::endl;
}