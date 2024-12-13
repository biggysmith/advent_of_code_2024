#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <queue>
#include <assert.h>

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a, const pos_t& b){ return std::tuple(a.x,a.y) < std::tuple(b.x,b.y); }
//bool operator==(const pos_t& a, const pos_t& b){ return std::tuple(a.x,a.y) == std::tuple(b.x,b.y); }
//bool operator!=(const pos_t& a, const pos_t& b){ return std::tuple(a.x,a.y) != std::tuple(b.x,b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x+b.x, a.y+b.y }; }
//pos_t operator-(const pos_t& a, const pos_t& b){ return { a.x-b.x, a.y-b.y }; }

struct map_t{
    std::vector<char> data;
    int width = 0;
    int height = 0;

    bool in_grid(const pos_t& p) const { return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height; }
    char get(const pos_t& p) const { return data[p.y * width + p.x]; }
    char& get(const pos_t& p) { return data[p.y * width + p.x]; }
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

auto flood(const map_t& map, map_t& taken, const pos_t& start) 
{ 
    std::set<pos_t> region;

    std::set<std::tuple<pos_t,pos_t>> edges;

    std::queue<pos_t> q;
    q.push(start);

    int outer = 0;
    int inner = 0;

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(region.count(curr)){
            continue;
        }
        
        region.insert(curr);

        auto diff = [&](const pos_t& p){
            return !map.in_grid(p) || map.get(p) != map.get(curr);
        };

        auto same = [&](const pos_t& p){
            return map.in_grid(p) && map.get(p) == map.get(curr);
        };     

        if(diff(curr + pos_t{0,-1}))   edges.insert({curr, curr + pos_t{0,-1}});
        if(diff(curr + pos_t{1,0}))   edges.insert({curr, curr + pos_t{1,0}});
        if(diff(curr + pos_t{0,1}))   edges.insert({curr, curr + pos_t{0,1}});
        if(diff(curr + pos_t{-1,0}))   edges.insert({curr, curr + pos_t{-1,0}});


        {
            // ..
            // .A
            if(diff(curr + pos_t{0,-1}) && diff(curr + pos_t{-1,0}) && diff(curr + pos_t{-1,-1})){
                outer++;
            }

            // ..
            // A.
            if(diff(curr + pos_t{0,-1}) && diff(curr + pos_t{1,0}) && diff(curr + pos_t{1,-1})){
                outer++;
            }

            // .A
            // ..
            if(diff(curr + pos_t{0,1}) && diff(curr + pos_t{-1,0}) && diff(curr + pos_t{-1,1})){
                outer++;
            }

            // A.
            // ..
            if(diff(curr + pos_t{0,1}) && diff(curr + pos_t{1,0}) && diff(curr + pos_t{1,1})){
                outer++;
            }


            //  A
            // A.
            if(diff(curr + pos_t{0,1}) && same(curr + pos_t{-1,1})){
                inner++;
            }

            // A 
            // .A
            if(diff(curr + pos_t{0,1}) && same(curr + pos_t{1,1})){
                inner++;
            }

            // A.
            //  A
            if(diff(curr + pos_t{0,-1}) && same(curr + pos_t{-1,-1})){
                inner++;
            }

            // .A
            // A 
            if(diff(curr + pos_t{0,-1}) && same(curr + pos_t{1,-1})){
                inner++;
            }
        }

        taken.get(curr) = 'x';

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t new_pos = curr + d;
            if(!region.count(new_pos) && map.in_grid(new_pos) && map.get(new_pos) == map.get(curr)){
                q.push(new_pos);
            }
        }
    }

    return std::make_tuple(region.size() * edges.size(), region.size() * (outer + inner));
} 

auto process(const map_t& map)
{
    map_t taken = map;

    size_t sum0 = 0;
    size_t sum1 = 0;

    for(int y=0; y<map.height; ++y){
        for(int x=0; x<map.width; ++x){

            if(taken.get({x,y}) != 'x'){
                auto [acc0, acc1] = flood(map, taken, {x, y});
                sum0 += acc0;
                sum1 += acc1;
            }

        }
    }

    return std::make_tuple(sum0, sum1);
}

void main()
{
    auto test_values = load_input("../src/day12/test_input.txt");
    auto actual_values = load_input("../src/day12/input.txt");

    auto [part1_a, part2_a] = process(test_values);
    auto [part1_b, part2_b] = process(actual_values);

    std::cout << "part1: " << part1_a << std::endl;
    std::cout << "part1: " << part1_b << std::endl;

    std::cout << "part2: " << part2_a << std::endl;
    std::cout << "part2: " << part2_b << std::endl;

    assert(part1_a == 1930);
    assert(part1_b == 1431440);
    assert(part2_a == 1206);
    assert(part2_b == 869070);

}