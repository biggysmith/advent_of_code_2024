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

bool operator<(const pos_t& a,const pos_t& b){ return std::tuple(a.x,a.y) < std::tuple(b.x,b.y); }
pos_t operator+(const pos_t& a,const pos_t& b){ return { a.x+b.x, a.y+b.y }; }
pos_t operator-(const pos_t& a,const pos_t& b){ return { a.x-b.x, a.y-b.y }; }

struct map_t{
    std::map<char, std::vector<pos_t>> antennas_to_positions;
    int width = 0;
    int height = 0;

    bool in_grid(const pos_t& p) const{
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
    }
};

map_t load_input(const std::string& file){
    map_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        for(int x=0; x<line.size(); ++x){
            if(line[x] != '.'){
                ret.antennas_to_positions[line[x]].push_back({ x, ret.height });
            }
        }
        ret.width = std::max(ret.width, (int)line.size());
        ret.height++;
    }
    return ret;
}

size_t part1(const map_t& map)
{
    std::set<pos_t> antinodes;

    for(auto& [antenna, positions] : map.antennas_to_positions)
    {
        for(int i=0; i<positions.size(); ++i) {
            for(int j=i+1; j<positions.size(); ++j) {

                pos_t delta = positions[i] - positions[j];

                if(map.in_grid(positions[i] + delta)) {
                    antinodes.insert(positions[i] + delta);
                }

                if(map.in_grid(positions[j] - delta)) {
                    antinodes.insert(positions[j] - delta);
                }

            }
        }
    }

    return antinodes.size();
}

size_t part2(const map_t& map)
{
    std::set<pos_t> antinodes;

    for(auto& [antenna, positions] : map.antennas_to_positions)
    {
        for(int i=0; i<positions.size(); ++i) {
            for(int j=i+1; j<positions.size(); ++j) {

                pos_t delta = positions[i] - positions[j];

                for(pos_t p = positions[i]; map.in_grid(p); p = p + delta){
                    antinodes.insert(p);
                }

                for(pos_t p = positions[j]; map.in_grid(p); p = p - delta){
                    antinodes.insert(p);
                }

            }
        }
    }

    return antinodes.size();
}

void main()
{
    auto test_values = load_input("../src/day08/test_input.txt");
    auto actual_values = load_input("../src/day08/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}