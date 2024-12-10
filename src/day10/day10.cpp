#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <queue>
#include <numeric>

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a, const pos_t& b){ return std::tuple(a.x,a.y) < std::tuple(b.x,b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x+b.x, a.y+b.y }; }

struct map_t{
    std::vector<int> heights;
    std::vector<pos_t> trailheads;
    int width = 0;
    int height = 0;

    bool in_grid(const pos_t& p) const{ return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height; }
    int get(const pos_t& p) const{ return heights[p.y * width + p.x]; }
};

map_t load_input(const std::string& file){
    map_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        for(int x=0; x<line.size(); ++x){
            ret.heights.push_back(line[x]-'0');
            if(line[x] == '0'){
                ret.trailheads.push_back({x, ret.height});
            }
        }
        ret.width = std::max(ret.width, (int)line.size());
        ret.height++;
    }
    return ret;
}

auto bfs(const map_t& map, const pos_t& trail_head)
{    
    std::map<pos_t, int> trail_ends;

    std::queue<pos_t> q;
    q.push(trail_head);

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(map.get(curr) == 9){
            trail_ends[curr]++;
            continue;
        }

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t new_pos = curr + d;
            if(map.in_grid(new_pos) && map.get(new_pos) == map.get(curr)+1){
                q.push(new_pos);
            }
        }
    }

    int rating = 0;
    for(auto& [end_pos, trail_count] : trail_ends){ rating += trail_count; }
    return std::make_pair(trail_ends.size(), rating);
}

auto process(const map_t& map)
{
    size_t score_sum = 0;
    size_t rating_sum = 0;

    for(auto& trailhead : map.trailheads){
        auto [score, rating] = bfs(map, trailhead);
        score_sum += score;
        rating_sum += rating;
    }

    return std::make_pair(score_sum, rating_sum);
}

void main()
{
    auto [test_score, test_rating] = process(load_input("../src/day10/test_input.txt"));
    std::cout << "part1: " << test_score << std::endl;
    std::cout << "part1: " << test_rating << std::endl;

    auto [actual_score, actual_rating] = process(load_input("../src/day10/input.txt"));
    std::cout << "part2: " << actual_score << std::endl;
    std::cout << "part2: " << actual_rating << std::endl;
}