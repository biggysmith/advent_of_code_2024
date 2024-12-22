#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <queue>
#include <set>

#undef NDEBUG
#include <assert.h>

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) < std::tie(b.x, b.y); }
bool operator==(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) == std::tie(b.x, b.y); }
bool operator!=(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) != std::tie(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t operator-(const pos_t& a, const pos_t& b){ return { a.x - b.x, a.y - b.y }; }

using key_to_pos_t = std::map<char, pos_t>;  

std::vector<std::string> load_input(const std::string& file){
    std::vector<std::string> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }
    return ret;
}

std::map<pos_t, char> dirs = { {{1,0},'>'}, {{0,-1},'^'}, {{-1,0},'<'}, {{0,1},'v'} };

struct big_int { int i=INT_MAX; };

auto bfs(const pos_t& start, const pos_t& end, const pos_t& max_pos, const pos_t& invalid)
{    
    std::map<pos_t,big_int> dist;
    std::map<pos_t, std::vector<std::string>> paths;

    std::queue<pos_t> q;
    q.push(start);

    dist[start].i = 0;
    paths[start].push_back({'A'}); // start from A

    auto in_bounds = [&](const pos_t& p){
        return p != invalid && p.x >= 0 && p.x <= max_pos.x && p.y >= 0 && p.y <= max_pos.y;
    };

    while(!q.empty()) {
        auto pos = q.front();
        q.pop();

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }) {
            pos_t new_pos = pos + d;

            if(in_bounds(new_pos)) {
                int new_dist = dist[pos].i + 1;
                if(new_dist < dist[new_pos].i) { // found smaller path
                    dist[new_pos].i = new_dist;
                    paths[new_pos].clear();
                    for(auto& path : paths[pos]) {
                        std::string new_path = path;
                        new_path.push_back(dirs[d]);
                        paths[new_pos].push_back(new_path);
                    }
                    q.push(new_pos);
                }else if(new_dist == dist[new_pos].i) { // found another same sized path
                    for(auto& path : paths[pos]) {
                        std::string new_path = path;
                        new_path.push_back(dirs[d]);
                        paths[new_pos].push_back(new_path);
                    }
                }
            }
        }
    }

    for(auto& path : paths[end]){
        path = path.substr(1) + path[0]; // want to end on A
    }
    return paths[end]; // all the shortest paths to end
}


struct num_path_t{
    pos_t src;
    pos_t dst;
};
bool operator<(const num_path_t& a, const num_path_t& b){ return std::tie(a.src, a.dst) < std::tie(b.src, b.dst); }

struct key_path_t{
    char src, dst;
};
bool operator<(const key_path_t& a, const key_path_t& b){ return std::tie(a.src, a.dst) < std::tie(b.src, b.dst); }

void key_paths(const std::string& code, int idx, std::string& curr_path, const key_to_pos_t& keys, const std::map<num_path_t, std::vector<std::string>>& paths, std::set<std::string>& path_set){
    if(idx == code.size()){
        path_set.insert(curr_path);
        return;
    }

    for(auto& path : paths.at({keys.at(idx-1<0 ? 'A' : code[idx-1]), keys.at(code[idx])})){
        key_paths(code, idx+1, curr_path + path, keys, paths, path_set);
    }
}

void key_paths2(const std::string& code, int idx, std::string& curr_path, const std::map<key_path_t, std::vector<std::string>>& paths, std::set<std::string>& path_set){
    if(idx == code.size()){
        path_set.insert(curr_path);
        return;
    }

    //for(auto& path : paths.at({keys.at(idx-1<0 ? 'A' : code[idx-1]), keys.at(code[idx])})){
    for(auto& path : paths.at({idx-1<0 ? 'A' : code[idx-1], code[idx]})){
        key_paths2(code, idx+1, curr_path + path, paths, path_set);
    }
}

struct cache_entry_t{
    std::string str;
    int depth;
};
bool operator<(const cache_entry_t& a, const cache_entry_t& b){ return std::tie(a.str, a.depth) < std::tie(b.str, b.depth); };

using cache_t = std::map<cache_entry_t, size_t>;

size_t key_path_length(const std::string& path, const key_to_pos_t& keys, const std::map<num_path_t, std::vector<std::string>>& paths, int depth, int max_depth, cache_t& cache){
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
        for(auto& sub : paths.at({keys.at(i-1<0 ? 'A' : path[i-1]), keys.at(path[i])})){
            sub_ret = std::min(sub_ret, key_path_length(sub, keys, paths, depth+1, max_depth, cache));
        }
        ret += sub_ret;
    }

    cache[{path,depth}] = ret;
    return ret;
}

size_t key_path_length2(const std::string& path, const std::map<key_path_t, std::vector<std::string>>& paths, int depth, int max_depth, cache_t& cache){
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
        for(auto& sub : paths.at({i-1<0 ? 'A' : path[i-1], path[i]})){
            sub_ret = std::min(sub_ret, key_path_length2(sub, paths, depth+1, max_depth, cache));
        }
        ret += sub_ret;
    }

    cache[{path,depth}] = ret;
    return ret;
}

size_t process(const std::vector<std::string>& codes, int num_bots)
{
    key_to_pos_t num_keys {
        {'7', { 0, 0 }}, {'8', { 1, 0 }}, {'9', { 2, 0 }},    
        {'4', { 0, 1 }}, {'5', { 1, 1 }}, {'6', { 2, 1 }},     
        {'1', { 0, 2 }}, {'2', { 1, 2 }}, {'3', { 2, 2 }}, 
                         {'0', { 1, 3 }}, {'A', { 2, 3 }} };

    key_to_pos_t dir_keys {
                         {'^', { 1, 0 }}, {'A', { 2, 0 }},
        {'<', { 0, 1 }}, {'v', { 1, 1 }}, {'>', { 2, 1 }} };

    std::map<num_path_t, std::vector<std::string>> num_paths;
    std::map<num_path_t, std::vector<std::string>> dir_paths;

    std::map<key_path_t, std::vector<std::string>> num_paths2;
    std::map<key_path_t, std::vector<std::string>> dir_paths2;

    for(auto i=dir_keys.begin(); i!=dir_keys.end(); ++i){
        for(auto j=i; j!=dir_keys.end(); ++j){
        
            {
                auto paths = bfs(i->second, j->second, { 2,1 }, { 0,0 });
                dir_paths[{i->second, j->second}].insert(
                    dir_paths[{i->second, j->second}].end(), paths.begin(), paths.end()
                );
                dir_paths2[{i->first, j->first}].insert(
                    dir_paths2[{i->first, j->first}].end(), paths.begin(), paths.end()
                );
            }

            {
                auto paths = bfs(j->second, i->second, { 2,1 }, { 0,0 });
                dir_paths[{j->second, i->second}].insert(
                    dir_paths[{j->second, i->second}].end(), paths.begin(), paths.end()
                );
                dir_paths2[{j->first, i->first}].insert(
                    dir_paths2[{j->first, i->first}].end(), paths.begin(), paths.end()
                );

            }

        }
    }

    for(auto i=num_keys.begin(); i!=num_keys.end(); ++i){
        for(auto j=i; j!=num_keys.end(); ++j){
        
            {
                auto paths = bfs(i->second, j->second, { 2,3 }, { 0,3 });
                num_paths[{i->second, j->second}].insert(
                    num_paths[{i->second, j->second}].end(), paths.begin(), paths.end()
                );
                num_paths2[{i->first, j->first}].insert(
                    num_paths2[{i->first, j->first}].end(), paths.begin(), paths.end()
                );
            }

            {
                auto paths = bfs(j->second, i->second, { 2,3 }, { 0,3 });
                num_paths[{j->second, i->second}].insert(
                    num_paths[{j->second, i->second}].end(), paths.begin(), paths.end()
                );
                num_paths2[{j->first, i->first}].insert(
                    num_paths2[{j->first, i->first}].end(), paths.begin(), paths.end()
                );
            }

        }
    }

    size_t sum = 0;
    for(auto& code : codes)
    {
        std::set<std::string> paths; 
        //key_paths(code, 0, std::string(), num_keys, num_paths, paths);
        key_paths2(code, 0, std::string(), num_paths2, paths);

        size_t len = UINT64_MAX;
        for(auto& path : paths){
            //len = std::min(len, key_path_length(path, dir_keys, dir_paths, 0, num_bots, cache_t()));
            len = std::min(len, key_path_length2(path, dir_paths2, 0, num_bots, cache_t()));
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

    assert(process(test_values, 2) == 126384);
    assert(process(actual_values, 2) == 157908);
    assert(process(test_values, 25) == 154115708116294);
    assert(process(actual_values, 25) == 196910339808654);

    // 166048 too high
    // 161014362301504 too low
    // 196910339808654 is correct
    // 413503376430618 too high
}