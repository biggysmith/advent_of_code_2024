#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <unordered_map>

#include <timer.hpp>

#undef NDEBUG
#include <assert.h>

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) < std::tie(b.x, b.y); }
bool operator==(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) == std::tie(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t rotate(const pos_t& dir, bool ccw){ return ccw ? pos_t{ dir.y, -dir.x } : pos_t{ -dir.y, dir.x }; }

struct map_t{
    std::vector<char> data;
    pos_t start, end;
    int width = 0;
    int height = 0;

    //char& get(const pos_t& p) { return data[p.y * width + p.x]; }
    char get(const pos_t& p) const { return data[p.y * width + p.x]; }
    bool in_grid(const pos_t& p) const { return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height; }
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

struct step_t {
    pos_t pos;
    pos_t dir;
    int score;
    int id;
};

bool operator>(const step_t& a, const step_t& b){ return std::tie(a.score, a.pos, a.dir) > std::tie(b.score, b.pos, b.dir); }

struct step_hash {
    size_t operator()(const pos_t& pos) const {
        return pos.x ^ (pos.y << 1);
    }
};

struct big_int{
    int i=INT_MAX;
};

auto process(const map_t& map) 
{
    //scoped_timer t;

    std::priority_queue<step_t, std::vector<step_t>, std::greater<>> pq;
    std::vector<std::vector<std::unordered_map<pos_t,big_int,step_hash>>> dist(map.height, std::vector<std::unordered_map<pos_t,big_int,step_hash>>(map.width));  // Distance map

    std::map<int, std::set<pos_t>> paths;
    int id = 0;

    pq.push({map.start, {1,0}, 0, 0}); 
    dist[map.start.x][map.start.y][{1,0}].i = 0;

    int shortestDistance = -1;

    std::unordered_set<pos_t,step_hash> super_set;
    super_set.insert(map.start);

    while (!pq.empty()) {
        auto [pos, dir, cost, curr_id] = pq.top();
        pq.pop();

        paths[curr_id].insert(pos);

        if (pos == map.end) {       
            if (shortestDistance == -1) {
                shortestDistance = cost;
            }
            if(cost == shortestDistance){
                super_set.insert(paths[curr_id].begin(), paths[curr_id].end());
            }
            continue;
        }

        for (auto& new_dir : { dir, rotate(dir, true), rotate(dir, false) }) {
            pos_t new_pos = pos + new_dir;
            int turn_cost = (new_dir == dir ? 1 : 1001);
            int new_cost = cost + turn_cost;

            if (map.in_grid(new_pos) && map.get(new_pos)=='.' && new_cost <= dist[new_pos.x][new_pos.y][new_dir].i) {
                if (new_cost <= dist[new_pos.x][new_pos.y][new_dir].i) {
                    //pq.push({newCost, nx, ny, newDir});
                    if(new_dir == dir){
                        pq.push({new_pos, new_dir, new_cost, curr_id});
                    }else{
                        id++;
                        pq.push({new_pos, new_dir, new_cost, id});
                        paths[id] = paths[curr_id];
                    }
                }
                dist[new_pos.x][new_pos.y][new_dir].i = new_cost;
            }
        }
    }

    return std::make_pair(shortestDistance, super_set.size());
}

void main()
{
    auto test_values0 = load_input("../src/day16/test_input0.txt");
    auto test_values1 = load_input("../src/day16/test_input1.txt");
    auto actual_values = load_input("../src/day16/input.txt");

    auto [score0, tiles0] = process(test_values0);
    std::cout << "part1: " << score0 << ", part2:" << tiles0 << std::endl;

    auto [score1, tiles1] = process(test_values1);
    std::cout << "part1: " << score1 << ", part2:" << tiles1 << std::endl;

    auto [score2, tiles2] = process(actual_values);
    std::cout << "part1: " << score2 << ", part2:" << tiles2 << std::endl;

    assert(score0 == 7036);
    assert(score1 == 11048);
    assert(score2 == 105496);

    assert(tiles0 == 45);
    assert(tiles1 == 64);
    assert(tiles2 == 524);
}