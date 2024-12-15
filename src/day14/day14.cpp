#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_set>
#include <algorithm>

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a,const pos_t& b){ return std::tuple(a.x,a.y) == std::tuple(b.x,b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t operator*(const pos_t& a, const pos_t& b){ return { a.x * b.x, a.y * b.y }; }
pos_t operator%(const pos_t& a, const pos_t& b){ return { a.x % b.x, a.y % b.y }; }

struct hasher {
    std::size_t operator()(const pos_t& pos) const { return pos.y*101 + pos.x; }
};

using set_t = std::unordered_set<pos_t,hasher>;

struct robot_t{
    pos_t p, v;
};

using robots_t = std::vector<robot_t>;

robots_t load_input(const std::string& file){
    robots_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(robot_t());
        sscanf_s(line.c_str(), "p=%d,%d v=%d,%d", &ret.back().p.x, &ret.back().p.y, &ret.back().v.x, &ret.back().v.y);
    }
    return ret;
}

pos_t mod(const pos_t& a, const pos_t& b) {
    return ((a % b) + b) % b;
}

int part1(robots_t robots, const pos_t& size)
{
    int quads[4] = { 0, 0, 0, 0 };

    for(auto& [pos, vel] : robots){
        pos = mod(pos + vel * pos_t{100, 100}, size);
        if(pos.x != size.x/2 && pos.y != size.y/2){
            int ix = pos.y < (size.y/2);
            int iy = pos.x < (size.x/2);
            quads[iy*2 + ix]++;
        }
    }

    return quads[0] * quads[1] * quads[2] * quads[3];
}

void dfs(int& cluster_size, const pos_t& curr, const set_t& points, set_t& visited) {
    if(visited.count(curr)){
        return;
    }
    visited.insert(curr);
    
    for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
        pos_t new_pos = curr + d;
        if(points.count(new_pos) && !visited.count(new_pos)){
            dfs(cluster_size += 1, new_pos, points, visited);
        }
    }
}

bool find_cluster(const robots_t& bots, int threshold) {
    set_t bot_set; // remember we have overlapping bots sometimes...
    for(auto& [pos, vel] : bots){
        bot_set.insert(pos);
    }

    set_t visited;
    for(auto& bot : bot_set) {
        int cluster_size = 0;
        dfs(cluster_size, bot, bot_set, visited);
        if(cluster_size >= threshold){
            return true;
        }
    }  
    return false;
}

int part2(robots_t bots, const pos_t& size)
{
    int seconds = 0;

    while(!find_cluster(bots, 40))
    {
        for(auto& [pos, vel] : bots){
            pos = mod(pos + vel, size);
        }
        seconds++;
    }

    return seconds;
}

void main()
{
    auto test_values = load_input("../src/day14/test_input.txt");
    auto actual_values = load_input("../src/day14/input.txt");

    std::cout << "part1: " << part1(test_values, { 11, 7 }) << std::endl;
    std::cout << "part1: " << part1(actual_values, { 101, 103 }) << std::endl;

    std::cout << "part2: " << part2(actual_values, { 101, 103 }) << std::endl;
}