#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <queue>
#include <timer.hpp>

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) < std::tie(b.x, b.y); }
bool operator==(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) == std::tie(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }

using bytes_t = std::map<pos_t,int>;

bytes_t load_input(const std::string& file){
    bytes_t ret;
    std::ifstream fs(file);
    int i=0;
    std::string line;
    while (std::getline(fs, line)) {
        pos_t pos;
        sscanf_s(line.c_str(), "%d,%d", &pos.x, &pos.y);
        ret[pos] = i++;
    }
    return ret;
}

bool in_bounds(const pos_t& pos, const pos_t& max_dims){
    return pos.x >= 0 && pos.x <= max_dims.x && pos.y >= 0 && pos.y <= max_dims.y;
}

bool byte_present(const bytes_t& bytes, const pos_t& pos, int ns){
    //return bytes.count(pos) && ns > bytes.at(pos);
    return bytes.count(pos) && bytes.at(pos) <= ns;
}

struct step_t{
    pos_t pos;
    int nanosec;
};

bool operator<(const step_t& a, const step_t& b){ return std::tie(a.pos) < std::tie(b.pos); }

auto bfs(const bytes_t& bytes, const pos_t& start, const pos_t& end, int byte_count)
{    
    std::queue<step_t> q;
    q.push({start,0});

    std::set<step_t> visited;

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(curr.pos == end){
            std::cout << curr.nanosec << std::endl;
            continue;
        }

        if(visited.count(curr)){
            continue;
        }
        visited.insert(curr);

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t new_pos = curr.pos + d;
            if(/*curr.nanosec < 12 &&*/ in_bounds(new_pos, end) && !byte_present(bytes, new_pos, byte_count)){
                q.push({new_pos, curr.nanosec+1});
            }
        }
    }

    return 0;
}

int part1(const bytes_t& bytes, const pos_t& end, int byte_count)
{
    std::cout << std::endl;

    bfs(bytes, {0,0}, end, byte_count);

    return 0;
}

auto bfs2(const bytes_t& bytes, const pos_t& start, const pos_t& end, int byte_count)
{    
    std::queue<pos_t> q;
    q.push(start);

    std::set<pos_t> visited;

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(curr == end){
            return true;
        }

        if(visited.count(curr)){
            continue;
        }
        visited.insert(curr);

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t new_pos = curr + d;
            if(in_bounds(new_pos, end) && !byte_present(bytes, new_pos, byte_count)){
                q.push(new_pos);
            }
        }
    }

    return false;
}

bool dfs(const bytes_t& bytes, const pos_t& curr, const pos_t& end, int byte_count, std::set<pos_t>& visited) 
{
    if (curr == end) {
        return true;
    }

    if(visited.count(curr)){
        return false;
    }
    visited.insert(curr);

    for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
        pos_t new_pos = curr + d;
        if(in_bounds(new_pos, end) && !byte_present(bytes, new_pos, byte_count)){
            bool found = dfs(bytes, new_pos, end, byte_count, visited);
            if(found){
                return true;
            }
        }
    }

    return false;
}


int part2(const bytes_t& bytes, const pos_t& end)
{
    scoped_timer t;

    for(int i=0; i<bytes.size(); ++i){
        std::set<pos_t> visited;
        bool found_exit = dfs(bytes, {0,0}, end, i, visited);
        if(!found_exit){
            for (auto& [pos, idx] : bytes) {
                if (idx == i) {
                    std::cout << "Blocked: " << pos.x << "," << pos.y << std::endl;
                    return 1;
                }
            }

        }
    }

    return 0;
}

void main()
{
    auto test_values = load_input("../src/day18/test_input.txt");
    auto actual_values = load_input("../src/day18/input.txt");

    std::cout << "part1: " << part1(test_values, {6,6}, 12) << std::endl;
    std::cout << "part1: " << part1(actual_values, {70,70}, 1024) << std::endl;

    std::cout << "part2: " << part2(test_values, {6,6}) << std::endl;
    std::cout << "part2: " << part2(actual_values, {70,70}) << std::endl;
}