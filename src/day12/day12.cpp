#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <queue>

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a, const pos_t& b){ return std::tuple(a.x,a.y) < std::tuple(b.x,b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x+b.x, a.y+b.y }; }
decltype(auto) operator+=(std::pair<size_t,size_t>& a, const std::pair<size_t,size_t>& b){ a.first += b.first; a.second += b.second; return a; }

struct map_t{
    std::vector<char> data;
    int width = 0;
    int height = 0;

    bool in_grid(const pos_t& p) const { return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height; }
    char get(const pos_t& p) const { return data[p.y * width + p.x]; }
};

map_t load_input(const std::string& file){
    map_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        for(auto c : line){
            ret.data.push_back(c);
        }
        ret.width = std::max(ret.width, (int)line.size());
        ret.height++;
    }
    return ret;
}

auto flood(const map_t& map, std::set<pos_t>& occupied, const pos_t& start) 
{ 
    int corners = 0;
    std::set<pos_t> plants;
    std::set<std::tuple<pos_t,pos_t>> edges;

    std::queue<pos_t> q;
    q.push(start);

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(plants.count(curr)){
            continue;
        }
        
        plants.insert(curr);
        occupied.insert(curr);

        auto diff = [&](const pos_t& p){ return !map.in_grid(p) || map.get(p) != map.get(curr); };
        auto same = [&](const pos_t& p){ return map.in_grid(p) && map.get(p) == map.get(curr); };      

        // count edges for perimeter
        if(diff(curr + pos_t{ 0, -1 })) { edges.insert({curr, curr + pos_t{ 0, -1 }}); } // top edge
        if(diff(curr + pos_t{ 1,  0 })) { edges.insert({curr, curr + pos_t{ 1,  0 }}); } // right edge
        if(diff(curr + pos_t{ 0,  1 })) { edges.insert({curr, curr + pos_t{ 0,  1 }}); } // bottom edge
        if(diff(curr + pos_t{-1,  0 })) { edges.insert({curr, curr + pos_t{-1,  0 }}); } // left edge

        // outer corners
        if(diff(curr + pos_t{ 0, -1 }) && diff(curr + pos_t{ -1, 0 }) && diff(curr + pos_t{ -1, -1 })) { corners++; } // top left outer
        if(diff(curr + pos_t{ 0, -1 }) && diff(curr + pos_t{  1, 0 }) && diff(curr + pos_t{  1, -1 })) { corners++; } // top right outer
        if(diff(curr + pos_t{ 0,  1 }) && diff(curr + pos_t{ -1, 0 }) && diff(curr + pos_t{ -1,  1 })) { corners++; } // bottom left outer
        if(diff(curr + pos_t{ 0,  1 }) && diff(curr + pos_t{  1, 0 }) && diff(curr + pos_t{  1,  1 })) { corners++; } // bottom right outer

        // inner corners
        if(diff(curr + pos_t{ 0,  1 }) && same(curr + pos_t{ -1,  1 })) { corners++; } // top left inner
        if(diff(curr + pos_t{ 0,  1 }) && same(curr + pos_t{  1,  1 })) { corners++; } // top right inner
        if(diff(curr + pos_t{ 0, -1 }) && same(curr + pos_t{ -1, -1 })) { corners++; } // bottom left inner
        if(diff(curr + pos_t{ 0, -1 }) && same(curr + pos_t{  1, -1 })) { corners++; } // bottom right inner

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t new_pos = curr + d;
            if(!plants.count(new_pos) && map.in_grid(new_pos) && map.get(new_pos) == map.get(curr)){
                q.push(new_pos);
            }
        }
    }

    return std::make_pair(plants.size()*edges.size(), plants.size()*corners);
} 

auto process(const map_t& map)
{
    std::set<pos_t> occupied;

    auto sums = std::make_pair(0ull, 0ull);

    for(int y=0; y<map.height; ++y){
        for(int x=0; x<map.width; ++x){

            if(!occupied.count({x,y})){
                sums += flood(map, occupied, {x, y});
            }

        }
    }

    return sums;
}

void main()
{
    auto test_values = load_input("../src/day12/test_input.txt");
    auto actual_values = load_input("../src/day12/input.txt");

    auto [test_price_1, test_price_2] = process(test_values);
    auto [price_1, price_2] = process(actual_values);

    std::cout << "part1: " << test_price_1 << std::endl;
    std::cout << "part1: " << price_1 << std::endl;

    std::cout << "part2: " << test_price_2 << std::endl;
    std::cout << "part2: " << price_2 << std::endl;
}