#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

struct pos_t{
    int x, y;
};

bool operator!=(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) != std::tie(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }

struct map_t{
    std::vector<char> data;
    pos_t start, end;
    int width = 0;
    int height = 0;

    char get(const pos_t& p) const { return data[p.y * width + p.x]; }
    bool in_grid(const pos_t& p) const { return p.x > 0 && p.x < width-1 && p.y > 0 && p.y < height-1; } // exclude edge
};

map_t load_input(const std::string& file){
    map_t ret;
    int mode = 0;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        for(int x=0; x<line.size(); ++x){
            if(line[x]=='S'){
                ret.start = {x, ret.height};
                ret.data.push_back('.');
            }else if(line[x]=='E'){
                ret.end = {x, ret.height};
                ret.data.push_back('.');
            }else{
                ret.data.push_back(line[x]);
            }
        }
        ret.width = std::max(ret.width, (int)line.size());
        ret.height++;
    }
    return ret;
}

int manhattan(const pos_t& a, const pos_t& b) {
    return std::abs(a.x-b.x) + std::abs(a.y-b.y);
}

std::vector<pos_t> find_path(const map_t& map)
{
    pos_t pos = map.start;
    pos_t prev_pos = {-1,-1};

    std::vector<pos_t> path;
    path.push_back(pos);

    while(pos != map.end){
        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t new_pos = pos + d;
            if(new_pos != prev_pos && map.in_grid(new_pos) && map.get(new_pos) == '.'){
                prev_pos = pos;
                pos = new_pos;
                path.push_back(pos);
                break;
            }
        }
    }

    return path;
}

int process(const map_t& map, int cheat_length, int min_saving)
{
    int sum = 0;
    std::vector<pos_t> path = find_path(map);

    for(int i=0; i<path.size(); ++i){
        for(int j=i+1; j<path.size(); ++j){

            int pico_second_dist = manhattan(path[i], path[j]);        
            if(pico_second_dist <= cheat_length){
                int saving = j - i - pico_second_dist;
                if(saving >= min_saving){
                    sum++;
                }
            }
        
        }
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day20/test_input.txt");
    auto actual_values = load_input("../src/day20/input.txt");

    std::cout << "part1 " << process(test_values, 2, 2) << std::endl;
    std::cout << "part1 " << process(actual_values, 2, 100) << std::endl;

    std::cout << "part2 " << process(test_values, 20, 50) << std::endl;
    std::cout << "part2 " << process(actual_values, 20, 100) << std::endl;
}