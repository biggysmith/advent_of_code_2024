#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <queue>
#include <set>
#include <map>

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) < std::tie(b.x, b.y); }
bool operator==(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) == std::tie(b.x, b.y); }
bool operator!=(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) != std::tie(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }

std::vector<std::string> load_input(const std::string& file){
    std::vector<std::string> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }
    return ret;
}

struct key_path_t{
    char src, dst;
};
bool operator<(const key_path_t& a, const key_path_t& b){ return std::tie(a.src, a.dst) < std::tie(b.src, b.dst); }

using paths_map_t = std::map<key_path_t, std::vector<std::string>>;

struct big_int { int i=INT_MAX; }; // need int type for map that defaults to INT_MAX for dist comparisons

auto bfs(const pos_t& start, const pos_t& end, const pos_t& max_pos, const pos_t& invalid, std::vector<std::string>& ab_paths)
{    
    std::map<pos_t, big_int> dist;
    std::map<pos_t, std::vector<std::string>> paths;

    std::queue<pos_t> q;
    q.push(start);

    dist[start].i = 0;
    paths[start].push_back({'A'}); // start from A

    auto valid_move = [&](const pos_t& p){
        return p != invalid && p.x >= 0 && p.x <= max_pos.x && p.y >= 0 && p.y <= max_pos.y;
    };

    std::map<pos_t, char> dirs = { {{1,0},'>'}, {{0,-1},'^'}, {{-1,0},'<'}, {{0,1},'v'} };

    while(!q.empty()) {
        auto pos = q.front();
        q.pop();

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }) {
            pos_t new_pos = pos + d;

            if(valid_move(new_pos)) {
                int new_dist = dist[pos].i + 1;
                if(new_dist < dist[new_pos].i) { // found smaller path
                    dist[new_pos].i = new_dist;
                    paths[new_pos].clear();
                    for(auto& path : paths[pos]) {
                        paths[new_pos].push_back(path + dirs[d]);
                    }
                    q.push(new_pos);
                }else if(new_dist == dist[new_pos].i) { // found another same sized path
                    for(auto& path : paths[pos]) {
                        paths[new_pos].push_back(path + dirs[d]);
                    }
                }
            }
        }
    }

    for(auto& path : paths[end]){
        path = path.substr(1) + path[0]; // want 'A' at end of path, so move first 'A' to end
        ab_paths.push_back(path); 
    }
}

void key_paths(const std::string& code, int idx, std::string& curr_path, const paths_map_t& paths_map, std::set<std::string>& code_paths_set){
    if(idx == code.size()){
        code_paths_set.insert(curr_path);
        return;
    }

    for(auto& path : paths_map.at({idx-1<0 ? 'A' : code[idx-1], code[idx]})){
        key_paths(code, idx+1, curr_path + path, paths_map, code_paths_set);
    }
}

struct cache_entry_t{
    std::string str;
    int depth;
};
bool operator<(const cache_entry_t& a, const cache_entry_t& b){ return std::tie(a.str, a.depth) < std::tie(b.str, b.depth); };

using cache_t = std::map<cache_entry_t, size_t>;

size_t min_key_path_length(const std::string& path, const paths_map_t& path_map, int depth, int max_depth, cache_t& cache){
    if(cache.count({path,depth})){
        return cache[{path,depth}];
    }

    if(depth == max_depth){
        cache[{path,depth}] = path.size();
        return path.size();
    }

    size_t ret = 0;
    for(int i=0; i<path.size(); ++i){
        size_t sub_ret = UINT64_MAX;
        for(auto& next_path : path_map.at({i-1<0 ? 'A' : path[i-1], path[i]})){
            sub_ret = std::min(sub_ret, min_key_path_length(next_path, path_map, depth+1, max_depth, cache));
        }
        ret += sub_ret;
    }

    cache[{path,depth}] = ret;
    return ret;
}

size_t process(const std::vector<std::string>& codes, int num_bots)
{
    std::map<char, pos_t> num_keys {
        {'7', { 0, 0 }}, {'8', { 1, 0 }}, {'9', { 2, 0 }},    
        {'4', { 0, 1 }}, {'5', { 1, 1 }}, {'6', { 2, 1 }},     
        {'1', { 0, 2 }}, {'2', { 1, 2 }}, {'3', { 2, 2 }}, 
                         {'0', { 1, 3 }}, {'A', { 2, 3 }} };

    std::map<char, pos_t> dir_keys {
                         {'^', { 1, 0 }}, {'A', { 2, 0 }},
        {'<', { 0, 1 }}, {'v', { 1, 1 }}, {'>', { 2, 1 }} };

    paths_map_t num_paths_map; // find all shortest paths between each number key
    for(auto i=num_keys.begin(); i!=num_keys.end(); ++i){
        for(auto j=i; j!=num_keys.end(); ++j){
            bfs(i->second, j->second, { 2,3 }, { 0,3 }, num_paths_map[{i->first, j->first}]); // i --> j
            bfs(j->second, i->second, { 2,3 }, { 0,3 }, num_paths_map[{j->first, i->first}]); // j --> i
        }
    }

    paths_map_t dir_paths_map; // find all shortest paths between each direction key
    for(auto i=dir_keys.begin(); i!=dir_keys.end(); ++i){
        for(auto j=i; j!=dir_keys.end(); ++j){
            bfs(i->second, j->second, { 2,1 }, { 0,0 }, dir_paths_map[{i->first, j->first}]); // i --> j
            bfs(j->second, i->second, { 2,1 }, { 0,0 }, dir_paths_map[{j->first, i->first}]); // j --> i
        }
    }

    size_t sum = 0;
    for(auto& code : codes)
    {
        std::set<std::string> code_paths_set; 
        key_paths(code, 0, std::string(), num_paths_map, code_paths_set); // starting num key path

        size_t len = UINT64_MAX;
        for(auto& path : code_paths_set){
            len = std::min(len, min_key_path_length(path, dir_paths_map, 0, num_bots, cache_t())); // robot num key path
        }
        sum += len * std::stoi(code.substr(0,3));
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day21/test_input.txt");
    auto actual_values = load_input("../src/day21/input.txt");

    std::cout << "part1: " << process(test_values, 2) << std::endl;
    std::cout << "part1: " << process(actual_values, 2) << std::endl;

    std::cout << "part2: " << process(test_values, 25) << std::endl;
    std::cout << "part2: " << process(actual_values, 25) << std::endl;
}