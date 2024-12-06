#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <algorithm>

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a,const pos_t& b){ return std::tuple(a.x,a.y) == std::tuple(b.x,b.y); }
pos_t operator+(const pos_t& a,const pos_t& b){ return { a.x+b.x, a.y+b.y }; }
pos_t rotate(const pos_t& dir){ return { -dir.y, dir.x }; }

struct pos_dir_t{
    pos_t pos, dir;
};

bool operator==(const pos_dir_t& a,const pos_dir_t& b){ return std::tuple(a.pos,a.dir) == std::tuple(b.pos,b.dir); }

struct hasher {
    std::size_t operator()(const pos_t& pos) const {
        return std::hash<int>()(pos.x) ^ std::hash<int>()(pos.y);
    }

    std::size_t operator()(const pos_dir_t& p) const {
        return hasher()(p.pos) ^ hasher()(p.dir);
    }
};

using set_t = std::unordered_set<pos_t, hasher>;

struct map_t{
    set_t obstacles;
    pos_t guard_pos;
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
            if(line[x] == '#'){
                ret.obstacles.insert({ x, ret.height });
            }else if(line[x] == '^'){
                ret.guard_pos = { x, ret.height };
            }
        }
        ret.width = std::max(ret.width, (int)line.size());
        ret.height++;
    }
    return ret;
}

set_t patrol(const map_t& map){
    pos_t curr_pos = map.guard_pos;
    pos_t dir = { 0, -1 };

    set_t visited;
    visited.insert(curr_pos);

    while(map.in_grid(curr_pos))
    {
        pos_t new_pos = curr_pos + dir;
        if(map.obstacles.count(new_pos)){
            dir = rotate(dir);
        }else{
            visited.insert(curr_pos = new_pos);
        }
    }
    visited.erase(curr_pos); // erase exit pos (outside grid)
    return visited;
}

size_t part1(const map_t& map)
{
    return patrol(map).size();
}

size_t part2(const map_t& map)
{
    set_t visited = patrol(map);
    visited.erase(map.guard_pos); // erase guard pos

    int loops = 0;

    for(auto& pos : visited)
    {
        map_t new_map = map;
        new_map.obstacles.insert(pos);

        pos_t curr_pos = map.guard_pos;
        pos_t dir = { 0, -1 };

        std::unordered_set<pos_dir_t, hasher> seen_obstacles;

        while(new_map.in_grid(curr_pos))
        {
            pos_t new_pos = curr_pos + dir;
            if(new_map.obstacles.count(new_pos)){
                if(seen_obstacles.count({new_pos, dir})){ 
                    loops++; // been here before from this direction == we have a loop
                    break;
                }
                seen_obstacles.insert({new_pos, dir});
                dir = { -dir.y, dir.x };
            }else{
                curr_pos = new_pos;
            }
        }
    }

    return loops;
}

void main()
{
    auto test_values = load_input("../src/day06/test_input.txt");
    auto actual_values = load_input("../src/day06/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}