#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <queue>

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) == std::tie(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }

struct hasher {
    std::size_t operator()(const pos_t& pos) const {
        return pos.y*71 + pos.x;
    }
};

using set_t = std::unordered_set<pos_t,hasher>;

struct bytes_t {
    std::vector<pos_t> list;
    std::unordered_map<pos_t,int,hasher> map;
    pos_t end_pos;

    bool in_bounds(const pos_t& pos) const { return pos.x >= 0 && pos.x <= end_pos.x && pos.y >= 0 && pos.y <= end_pos.y; }
    bool corrupted(const pos_t& pos, int nanosec) const { return map.count(pos) && map.at(pos) < nanosec; }
};

bytes_t load_input(const std::string& file){
    bytes_t ret;
    std::ifstream fs(file);
    int i=0;
    ret.end_pos = { INT_MIN, INT_MIN };
    std::string line;
    while (std::getline(fs, line)) {
        pos_t pos;
        sscanf_s(line.c_str(), "%d,%d", &pos.x, &pos.y);
        ret.map[pos] = i++;
        ret.list.push_back(pos);
        ret.end_pos = { std::max(ret.end_pos.x, pos.x), std::max(ret.end_pos.y, pos.y) };
    }
    return ret;
}

struct step_t{
    pos_t pos;
    int nanosec;
};

auto bfs(const bytes_t& bytes, int byte_count)
{    
    std::queue<step_t> q;
    q.push({{0,0}, 0});

    set_t visited;

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(curr.pos == bytes.end_pos){
            return curr.nanosec;
        }

        if(visited.count(curr.pos)){
            continue;
        }
        visited.insert(curr.pos);

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t new_pos = curr.pos + d;
            if(bytes.in_bounds(new_pos) && !bytes.corrupted(new_pos, byte_count)){
                q.push({new_pos, curr.nanosec+1});
            }
        }
    }

    return 0;
}

int part1(const bytes_t& bytes, int byte_count)
{
    return bfs(bytes, byte_count);
}

bool dfs(const bytes_t& bytes, const pos_t& curr, int byte_count, set_t& visited) 
{
    if (curr == bytes.end_pos) {
        return true;
    }

    visited.insert(curr);

    for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
        pos_t new_pos = curr + d;
        if(!visited.count(new_pos) && bytes.in_bounds(new_pos) && !bytes.corrupted(new_pos, byte_count)){
            if(dfs(bytes, new_pos, byte_count, visited)){
                return true;
            }
        }
    }

    visited.erase(curr);
    return false;
}

std::string part2(const bytes_t& bytes)
{
    pos_t pos;
    for(int i=0; i<bytes.list.size(); ++i){
        set_t visited;
        if(!dfs(bytes, {0,0}, i, visited)) {
            pos = bytes.list[i-1];
            break;
        }
    }
    return std::to_string(pos.x) + "," + std::to_string(pos.y);
}

void main()
{
    auto test_values = load_input("../src/day18/test_input.txt");
    auto actual_values = load_input("../src/day18/input.txt");

    std::cout << "part1: " << part1(test_values, 12) << std::endl;
    std::cout << "part1: " << part1(actual_values, 1024) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}