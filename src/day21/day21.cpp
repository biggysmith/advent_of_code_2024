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
    //std::map<pos_t, std::vector<std::vector<pos_t>>> paths;
    std::map<pos_t, std::vector<std::vector<pos_t>>> paths;

    std::queue<pos_t> q;
    q.push(start);

    dist[start].i = 0;
    paths[start].push_back({start});

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
                        std::vector<pos_t> newPath = path;
                        newPath.push_back(new_pos);
                        paths[new_pos].push_back(newPath);
                    }
                    q.push(new_pos);
                }else if(new_dist == dist[new_pos].i) { // found another same sized path
                    for(auto& path : paths[pos]) {
                        std::vector<pos_t> newPath = path;
                        newPath.push_back(new_pos);
                        paths[new_pos].push_back(newPath);
                    }
                }
            }
        }
    }

    return paths[end]; // all the shortest paths to end
}


auto bfs2(const pos_t& start, const pos_t& end, const pos_t& max_pos, const pos_t& invalid)
{    
    std::map<pos_t,big_int> dist;
    //std::map<pos_t, std::vector<std::vector<pos_t>>> paths;
    std::map<pos_t, std::vector<std::string>> paths;

    std::queue<pos_t> q;
    q.push(start);

    dist[start].i = 0;
    //paths[start].push_back({start});
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

void key_paths(const std::string& code, int idx, std::string& path, const key_to_pos_t& keys, const std::map<num_path_t, std::vector<std::string>>& paths, std::set<std::string>& path_set){
    if(idx == code.size()){
        path_set.insert(path);
        return;
    }

    for(auto& sub_path : paths.at({keys.at(idx-1<0 ? 'A' : code[idx-1]), keys.at(code[idx])})){
        key_paths(code, idx+1, path + sub_path, keys, paths, path_set);
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

    std::map<num_path_t, std::string> dir_paths;
    std::map<num_path_t, std::vector<std::string>> dir_paths_many;

    for(auto i=dir_keys.begin(); i!=dir_keys.end(); ++i){
        for(auto j=i; j!=dir_keys.end(); ++j){
        
            {
                //auto paths2 = bfs(i->second, j->second, { 2,1 }, { 0,0 });
                auto paths3 = bfs2(i->second, j->second, { 2,1 }, { 0,0 });
                dir_paths_many[{i->second, j->second}].insert(
                    dir_paths_many[{i->second, j->second}].end(), paths3.begin(), paths3.end()
                );

                /*for(auto& path2 : paths2){
                    std::string path_str;
                    for(int i=1; i<path2.size(); ++i) {
                        path_str += dirs[path2[i] - path2[i-1]];
                    }
                    path_str += 'A';

                    dir_paths_many[{i->second, j->second}].push_back(path_str);
                }*/
            }

            {
                /*auto paths2 = bfs(j->second, i->second, { 2,1 }, { 0,0 });

                for(auto& path2 : paths2){
                    std::string path_str;
                    for(int i=1; i<path2.size(); ++i) {
                        path_str += dirs[path2[i] - path2[i-1]];
                    }
                    path_str += 'A';

                    dir_paths_many[{j->second, i->second}].push_back(path_str);
                }*/

                auto paths3 = bfs2(j->second, i->second, { 2,1 }, { 0,0 });
                dir_paths_many[{j->second, i->second}].insert(
                    dir_paths_many[{j->second, i->second}].end(), paths3.begin(), paths3.end()
                );

            }

        }
    }

    for(auto i=num_keys.begin(); i!=num_keys.end(); ++i){
        for(auto j=i; j!=num_keys.end(); ++j){
        
            {
                auto paths = bfs(i->second, j->second, { 2,3 }, { 0,3 });

                for(auto& path : paths){
                    std::string path_str;
                    for(int i = 1; i<path.size(); ++i) {
                        path_str += dirs[path[i] - path[i-1]];
                    }
                    path_str += 'A';

                    num_paths[{i->second, j->second}].push_back(path_str);

                }
            }

            {
                auto paths = bfs(j->second, i->second, { 2,3 }, { 0,3 });

                for(auto& path : paths){
                    std::string path_str;
                    for(int i = 1; i<path.size(); ++i) {
                        path_str += dirs[path[i] - path[i-1]];
                    }
                    path_str += 'A';

                    num_paths[{j->second, i->second}].push_back(path_str);
                }
            }

        }
    }

    size_t sum = 0;
    for(auto& code : codes)
    {
        std::set<std::string> paths; 
        key_paths(code, 0, std::string(), num_keys, num_paths, paths);

        size_t len3 = UINT64_MAX;
        for(auto& path : paths){
            size_t new_len = key_path_length(path, dir_keys, dir_paths_many, 0, num_bots, cache_t());
            len3 = std::min(len3, new_len);
        }

        size_t num = std::stoi(code.substr(0,3));
        sum += len3*num;
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