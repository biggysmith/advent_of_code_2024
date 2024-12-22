#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <queue>
#include <set>

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) < std::tie(b.x, b.y); }
bool operator==(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) == std::tie(b.x, b.y); }
bool operator!=(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) != std::tie(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t operator-(const pos_t& a, const pos_t& b){ return { a.x - b.x, a.y - b.y }; }
std::ostream& operator<<(std::ostream& os, const pos_t& pos) { os << pos.x << "," << pos.y; return os; }

using codes_t = std::vector<std::string>;

codes_t load_input(const std::string& file){
    codes_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }
    return ret;
}

/*

+---+---+---+
| 7 | 8 | 9 |
+---+---+---+
| 4 | 5 | 6 |
+---+---+---+
| 1 | 2 | 3 |
+---+---+---+
    | 0 | A |
    +---+---+

    +---+---+
    | ^ | A |
+---+---+---+
| < | v | > |
+---+---+---+

    +---+---+
    | ^ | A |
+---+---+---+
| < | v | > |
+---+---+---+

    +---+---+
    | ^ | A |
+---+---+---+
| < | v | > |
+---+---+---+

+---+---+---+---+---+---+---+---+---+---+---+---+
| 7 | 8 | 9 | 4 | 5 | 6 | 1 | 2 | 3     | 0 | A |
+---+---+---+---+---+---+---+---+---+---+---+---+

1,3 --> 2,3

*/

struct bimap_t {
    std::map<char, pos_t> key_to_pos;
    std::map<pos_t, char> pos_to_key;

    void insert(char key, const pos_t& pos) {
        key_to_pos[key] = pos;
        pos_to_key[pos] = key;
    }
    char get_key(const pos_t& pos) const { return pos_to_key.at(pos); }
    pos_t get_pos(const char& key) const { return key_to_pos.at(key); }
};

struct path_t{
    std::vector<pos_t> positions;
    std::vector<char> keys;
};

struct pos_dir_t{
    pos_t pos;
    pos_t dir;
};

std::map<pos_t, char> dirs = { {{1,0},'>'}, {{0,-1},'^'}, {{-1,0},'<'}, {{0,1},'v'} };

auto bfs(const bimap_t& map, const pos_t& max_pos, const pos_t& invalid, const pos_t& start, const pos_t& end)
{    
    std::queue<pos_t> q;
    q.push(start);

    std::set<pos_t> visited;
    visited.insert(start);

    std::map<pos_t, pos_t> parent;

    auto in_bounds = [&](const pos_t& p){
        if(p == invalid){
            return false;
        }
        return p.x >= 0 && p.x <= max_pos.x && p.y >= 0 && p.y <= max_pos.y;
    };

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(curr == end){
            std::vector<pos_t> path;
            for(pos_t v=end; v!=start; v=parent[v]) {
                path.push_back(v);
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        //path += map.pos_to_key.at(curr);

        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t new_pos = curr + d;
            if(!visited.count(new_pos) && in_bounds(new_pos)){
                visited.insert(new_pos);
                parent[new_pos] = curr;
                q.push(new_pos);
            }
        }
    }
    return std::vector<pos_t>();
}

struct path_node_t{
    pos_t a, b;
};

auto bfs3(const bimap_t& map, const pos_t& max_pos, const pos_t& invalid, const pos_t& start, const pos_t& end)
{    
    int rows = 3, cols = 4;
    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, INT_MAX)); // Distance array
    std::vector<std::vector<std::vector<std::vector<pos_t>>>> paths(rows, std::vector<std::vector<std::vector<pos_t>>>(cols)); // Paths array

    std::vector<pos_t> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // Right, Down, Left, Up

    std::queue<pos_t> q;
    q.push(start);
    dist[start.x][start.y] = 0;
    paths[start.x][start.y].push_back({start});

    auto in_bounds = [&](const pos_t& p){
        if(p == invalid){
            return false;
        }
        return p.x >= 0 && p.x <= max_pos.x && p.y >= 0 && p.y <= max_pos.y;
    };

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for (auto [dx, dy] : directions) {
            int nx = x + dx, ny = y + dy;

            //if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && grid[nx][ny] == 0) { // Valid cell
            if(in_bounds({nx,ny})){
                int newDist = dist[x][y] + 1;

                if (newDist < dist[nx][ny]) {
                    dist[nx][ny] = newDist;
                    paths[nx][ny].clear(); // Clear previous paths
                    for (const auto& path : paths[x][y]) {
                        std::vector<pos_t> newPath = path;
                        newPath.push_back({nx, ny});
                        paths[nx][ny].push_back(newPath);
                    }
                    q.push({nx, ny});
                } else if (newDist == dist[nx][ny]) {
                    for (const auto& path : paths[x][y]) {
                        std::vector<pos_t> newPath = path;
                        newPath.push_back({nx, ny});
                        paths[nx][ny].push_back(newPath);
                    }
                }
            }
        }
    }

    return paths[end.x][end.y];
}


struct num_path_t{
    pos_t src;
    pos_t dst;
};
bool operator<(const num_path_t& a, const num_path_t& b){ return std::tie(a.src, a.dst) < std::tie(b.src, b.dst); }

struct pos2_t{
    pos_t pos;
    pos_t dir;
    int dist;
};
bool operator<(const pos2_t& a, const pos2_t& b){ return std::tie(a.dist) > std::tie(b.dist); }

struct big_int { int i=INT_MAX; };

auto bfs2(const bimap_t& map, const std::map<num_path_t, std::string>& num_paths, const pos_t& max_pos, const pos_t& invalid, const pos_t& start, const pos_t& end)
{    
    std::priority_queue<pos2_t> q;
    q.push({start, {2,0}, 0});

    std::set<pos_t> visited;
    visited.insert(start);

    std::map<pos_t,big_int> dist;
    dist[start].i = 0;

    std::map<pos_t, pos_t> parent;

    /*if(start == pos_t{2,0} && end == pos_t{0,1}){
        std::cout << "hello" << std::endl;
    }*/

    auto in_bounds = [&](const pos_t& p){
        if(p == invalid){
            return false;
        }
        return p.x >= 0 && p.x <= max_pos.x && p.y >= 0 && p.y <= max_pos.y;
    };

    //pos_t dir_pos = {2,0};

    while (!q.empty()) 
    {
        auto curr = q.top();
        q.pop();

        if (curr.dist > dist[curr.pos].i) continue;

        if(curr.pos == end){
            std::vector<pos_t> path;
            for(pos_t v=end; v!=start; v=parent[v]) {
                path.push_back(v);
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        //path += map.pos_to_key.at(curr);



        for(auto& d : { pos_t{0, 1}, pos_t{1, 0}, pos_t{0, -1}, pos_t{-1, 0} }){
            pos_t dir_pos;
            if(d == pos_t{0, 1}){
                dir_pos = { 1, 0 };
            }else if(d == pos_t{1, 0}){
                dir_pos = { 2, 1 };
            }else if(d == pos_t{0, -1}){
                dir_pos = { 1, 1 };
            }else if(d == pos_t{-1, 0}){
                dir_pos = { 0, 1 };
            }else /*if(d == pos_t{-1, 0})*/{
                dir_pos = { 2, 0 };
            }
            pos_t new_pos = curr.pos + d;
            /*int weight = (int)num_paths.at({dir_pos, dir_pos}).size();*/
            //if(/*!visited.count(new_pos) &&*/ in_bounds(new_pos) && (dist[curr.pos].i + (int)num_paths.at({curr.pos, new_pos}).size()) < dist[new_pos].i){
            if(/*!visited.count(new_pos) &&*/ in_bounds(new_pos)){
                int weight = (int)num_paths.at({curr.dir, dir_pos}).size();
                //int weight = 1;
                if((dist[curr.pos].i + weight) < dist[new_pos].i){
                    //visited.insert(new_pos);
                    dist[new_pos].i = dist[curr.pos].i + weight;
                    parent[new_pos] = curr.pos;
                    q.push({new_pos, dir_pos, dist[new_pos].i});
                }
            }
        }
    }
    return std::vector<pos_t>();
}


std::string inverse(const std::string& str){
    std::string ret;
    for(auto c : str){
        ret += c=='<' ? '>' :
               c=='>' ? '<' :
               c=='^' ? 'v' :
               c=='v' ? '^' : 'A';
    }
    return ret;
}

std::string process_key_pad(const std::string& code, const bimap_t& keys, const std::map<num_path_t, std::string>& paths){
    std::string path;
    path += paths.at({keys.key_to_pos.at('A'), keys.key_to_pos.at(code[0])});
    for(int i=1; i<code.size(); ++i){
        char a = code[i-1];
        char b = code[i];
        path += paths.at({keys.key_to_pos.at(a), keys.key_to_pos.at(b)});
    }
    return path;
}

void process_key_pad_many(const std::string& code, int idx, std::string& path, const bimap_t& keys, const std::map<num_path_t, std::vector<std::string>>& paths, std::set<std::string>& path_set){

    if(idx == code.size()){
        path_set.insert(path);
        return;
    }

    for(auto& sub_path : paths.at({keys.key_to_pos.at(idx-1<0 ? 'A' : code[idx-1]), keys.key_to_pos.at(code[idx])})){
        process_key_pad_many(code, idx+1, path + sub_path, keys, paths, path_set);
    }
}

std::string process_key_pad_recursive(const std::string& code, const bimap_t& keys, const std::map<num_path_t, std::string>& paths, int depth, int max_depth){
    std::string path;
    path += paths.at({keys.key_to_pos.at('A'), keys.key_to_pos.at(code[0])});
    for(int i=1; i<code.size(); ++i){
        char a = code[i-1];
        char b = code[i];
        path += paths.at({keys.key_to_pos.at(a), keys.key_to_pos.at(b)});
    }
    if(depth < max_depth){
        path = process_key_pad_recursive(path, keys, paths, depth+1, max_depth);
    }
    return path;
}

std::string process_key_pad_recursive2(const std::string& code, const bimap_t& keys, const std::map<num_path_t, std::string>& paths, int depth, int max_depth){
    if(depth == max_depth){
        return code;
    }

    std::string path;
    path += process_key_pad_recursive2(paths.at({keys.key_to_pos.at('A'), keys.key_to_pos.at(code[0])}), keys, paths, depth+1, max_depth);
    for(int i=1; i<code.size(); ++i){
        char a = code[i-1];
        char b = code[i];
        path += process_key_pad_recursive2(paths.at({keys.key_to_pos.at(a), keys.key_to_pos.at(b)}), keys, paths, depth+1, max_depth);
    }
    return path;
}

struct cache_entry_t{
    std::string str;
    int depth;
};
bool operator<(const cache_entry_t& a, const cache_entry_t& b){ return std::tie(a.str, a.depth) < std::tie(b.str, b.depth); };

using cache_t = std::map<cache_entry_t, size_t>;

size_t process_key_pad_recursive_size(const std::string& code, const bimap_t& keys, const std::map<num_path_t, std::string>& paths, int depth, int max_depth, cache_t& cache){
    if(cache.count({code,depth})){
        //std::cout << "cached!" << std::endl; 
        return cache[{code,depth}];
    }

    if(depth == max_depth){
        cache[{code,depth}] = code.size();
        return code.size();
    }

    size_t ret = 0;
    ret += process_key_pad_recursive_size(paths.at({keys.key_to_pos.at('A'), keys.key_to_pos.at(code[0])}), keys, paths, depth+1, max_depth, cache);
    for(int i=1; i<code.size(); ++i){
        char a = code[i-1];
        char b = code[i];
        ret += process_key_pad_recursive_size(paths.at({keys.key_to_pos.at(a), keys.key_to_pos.at(b)}), keys, paths, depth+1, max_depth, cache);
    }

    cache[{code,depth}] = ret;
    return ret;
}

size_t process_key_pad_recursive_size_smallest(const std::string& code, const bimap_t& keys, const std::map<num_path_t, std::vector<std::string>>& paths, int depth, int max_depth, cache_t& cache){
    if(cache.count({code,depth})){
        //std::cout << "cached!" << std::endl; 
        return cache[{code,depth}];
    }

    if(depth == max_depth){
        cache[{code,depth}] = code.size();
        return code.size();
    }

    size_t ret = 0;
    size_t sub_ret = UINT64_MAX;
    //ret += process_key_pad_recursive_size_smallest(paths.at({keys.key_to_pos.at('A'), keys.key_to_pos.at(code[0])}), keys, paths, depth+1, max_depth, cache);
    for(auto& sub : paths.at({keys.key_to_pos.at('A'), keys.key_to_pos.at(code[0])})){
        sub_ret = std::min(sub_ret, process_key_pad_recursive_size_smallest(sub, keys, paths, depth+1, max_depth, cache));
    }
    ret += sub_ret;
    for(int i=1; i<code.size(); ++i){
        char a = code[i-1];
        char b = code[i];
        size_t sub_ret = UINT64_MAX;
        for(auto& sub : paths.at({keys.key_to_pos.at(a), keys.key_to_pos.at(b)})){
            sub_ret = std::min(sub_ret, process_key_pad_recursive_size_smallest(sub, keys, paths, depth+1, max_depth, cache));
        }
        ret += sub_ret;
    }

    cache[{code,depth}] = ret;
    return ret;
}

std::string process_num_pad(const std::string& code, const bimap_t& keys, const std::map<num_path_t, std::string>& paths){
    std::string path;
    path += paths.at({keys.key_to_pos.at('A'), keys.key_to_pos.at(code[0])});
    for(int i=1; i<code.size(); ++i){
        char a = code[i-1];
        char b = code[i];
        path += paths.at({keys.key_to_pos.at(a), keys.key_to_pos.at(b)});
    }
    return path;
}

size_t part1(const codes_t& codes, int num_bots)
{
    /*

    
1,3 --> 2,3

      0   1   2
    +---+---+---+
  0 | 7 | 8 | 9 |
    +---+---+---+
  1 | 4 | 5 | 6 |
    +---+---+---+
  2 | 1 | 2 | 3 |
    +---+---+---+
  3     | 0 | A |
        +---+---+

      0   1   2
        +---+---+
  0     | ^ | A |
    +---+---+---+
  1 | < | v | > |
    +---+---+---+

    */

    /*std::vector<pos_t> num_keys {
        { 1, 3 }, // 0
        { 2, 0 }, // 1
        { 2, 1 }, // 2
        { 2, 2 }, // 3
        { 1, 0 }, // 4
        { 1, 1 }, // 5
        { 1, 2 }, // 6
        { 0, 0 }, // 7
        { 0, 1 }, // 8
        { 0, 2 }, // 9
        { 2, 3 }, // A
    };

    std::map<char, pos_t> num_keys2 {
        { '0', { 1, 3 } },
        { '1', { 2, 0 } },
        { '2', { 2, 1 } },
        { '3', { 2, 2 } },
        { '4', { 1, 0 } },
        { '5', { 1, 1 } },
        { '6', { 1, 2 } },
        { '7', { 0, 0 } },
        { '8', { 0, 1 } },
        { '9', { 0, 2 } },
        { 'A', { 2, 3 } },
    };

    std::map<char, pos_t> dir_keys2 {
        { '^', { 1, 0 } },
        { '>', { 2, 1 } },
        { 'v', { 1, 1 } },
        { '<', { 0, 1 } },
        { 'A', { 0, 2 } },
    };
    

      0   1   2
    +---+---+---+
  0 | 7 | 8 | 9 |
    +---+---+---+
  1 | 4 | 5 | 6 |
    +---+---+---+
  2 | 1 | 2 | 3 |
    +---+---+---+
  3     | 0 | A |
        +---+---+
    
      0   1   2
        +---+---+
  0     | ^ | A |
    +---+---+---+
  1 | < | v | > |
    +---+---+---+
    
    */

    bimap_t num_keys;
    num_keys.insert('0', { 1, 3 });
    num_keys.insert('1', { 0, 2 });
    num_keys.insert('2', { 1, 2 });
    num_keys.insert('3', { 2, 2 });
    num_keys.insert('4', { 0, 1 });
    num_keys.insert('5', { 1, 1 });
    num_keys.insert('6', { 2, 1 });
    num_keys.insert('7', { 0, 0 });
    num_keys.insert('8', { 1, 0 });
    num_keys.insert('9', { 2, 0 });
    num_keys.insert('A', { 2, 3 });

    bimap_t dir_keys;
    dir_keys.insert('^', { 1, 0 });
    dir_keys.insert('>', { 2, 1 });
    dir_keys.insert('v', { 1, 1 });
    dir_keys.insert('<', { 0, 1 });
    dir_keys.insert('A', { 2, 0 });

    //std::map<num_path_t, std::string> num_paths;
    std::map<num_path_t, std::vector<std::string>> num_paths;


    std::map<num_path_t, std::string> dir_paths0;
    std::map<num_path_t, std::vector<std::string>> dir_paths_many;

    for(auto i=dir_keys.key_to_pos.begin(); i!=dir_keys.key_to_pos.end(); ++i){
        for(auto j=i; j!=dir_keys.key_to_pos.end(); ++j){
        
            {
                auto path = bfs(dir_keys, { 2,1 }, { 0,0 }, i->second, j->second);
                auto paths2 = bfs3(dir_keys, { 2,1 }, { 0,0 }, i->second, j->second);

                //for(auto& path : paths){
                    std::string path_str;
                    for(int i=1; i<path.size(); ++i) {
                        path_str += dirs[path[i] - path[i-1]];
                    }
                    path_str += 'A';

                    dir_paths0[{i->second, j->second}] = path_str;
                    //std::cout << "'" << i->first << "': " << i->second << " --> " <<  "'" << j->first << "': " << j->second << " path: " << path_str  << std::endl;
                //}

                for(auto& path2 : paths2){
                    std::string path_str;
                    for(int i=1; i<path2.size(); ++i) {
                        path_str += dirs[path2[i] - path2[i-1]];
                    }
                    path_str += 'A';

                    dir_paths_many[{i->second, j->second}].push_back(path_str);
                }
            }

            {
                auto path = bfs(dir_keys, { 2,1 }, { 0,0 }, j->second, i->second);
                auto paths2 = bfs3(dir_keys, { 2,1 }, { 0,0 }, j->second, i->second);

                std::string path_str;
                for(int i=1; i<path.size(); ++i) {
                    path_str += dirs[path[i] - path[i-1]];
                }
                path_str += 'A';

                dir_paths0[{j->second, i->second}] = path_str;
                //std::cout << "'" << j->first << "': " << j->second << " --> " <<  "'" << i->first << "': " << i->second << " path: " << path_str  << std::endl;

                for(auto& path2 : paths2){
                    std::string path_str;
                    for(int i=1; i<path2.size(); ++i) {
                        path_str += dirs[path2[i] - path2[i-1]];
                    }
                    path_str += 'A';

                    dir_paths_many[{j->second, i->second}].push_back(path_str);
                }

            }
            
            //dir_paths[{j->second, i->second}] = inverse(path_str);

            //std::cout << "'" << i->first << "': " << i->second << " --> " <<  "'" << j->first << "': " << j->second << " path: " << path_str  << std::endl;
            //std::cout << "'" << j->first << "': " << j->second << " --> " <<  "'" << i->first << "': " << i->second << " path: " << inverse(path_str)  << std::endl;

        }
    }

    auto dir_paths = dir_paths0;
    /*for(auto& [path, str] : dir_paths){
        std::cout << str << " -> ";
        //str = process_key_pad(str, dir_keys, dir_paths0);
        //str = process_key_pad(str, dir_keys, dir_paths0);
        std::cout << str << std::endl;
    }*/

    //int num_bots = 25;

    //std::cout << std::endl;

    for(auto i=num_keys.key_to_pos.begin(); i!=num_keys.key_to_pos.end(); ++i){
        for(auto j=i; j!=num_keys.key_to_pos.end(); ++j){
        
            {
                auto paths = bfs3(num_keys, { 2,3 }, { 0,3 }, i->second, j->second);

                for(auto& path : paths){
                    std::string path_str;
                    for(int i = 1; i<path.size(); ++i) {
                        path_str += dirs[path[i] - path[i-1]];
                    }
                    path_str += 'A';

                #if 0
                    if(num_paths.count({i->second, j->second})){
                    #if 0
                        std::string path_str0 = process_num_pad(path_str, dir_keys, dir_paths);
                        path_str0 = process_num_pad(path_str0, dir_keys, dir_paths);

                        std::string path_str1 = process_num_pad(num_paths[{i->second, j->second}], dir_keys, dir_paths);
                        path_str1 = process_num_pad(path_str1, dir_keys, dir_paths);

                        if(path_str0.size() < path_str1.size()){
                            num_paths[{i->second, j->second}] = path_str;
                        }
                    #endif

                        size_t size0 = process_key_pad_recursive_size(path_str, dir_keys, dir_paths, 0, num_bots, cache_t());
                        size_t size1 = process_key_pad_recursive_size(num_paths[{i->second, j->second}], dir_keys, dir_paths, 0, num_bots, cache_t());

                        if(size0 < size1){
                            num_paths[{i->second, j->second}] = path_str;
                        }

                        //std::cout << "'" << i->first << "': " << i->second << " --> " <<  "'" << j->first << "': " << j->second << " path: " << path_str  << std::endl;
                    }else{
                        num_paths[{i->second, j->second}] = path_str;
                    }
                #endif


                    num_paths[{i->second, j->second}].push_back(path_str);

                }
            }

            {
                auto paths = bfs3(num_keys, { 2,3 }, { 0,3 }, j->second, i->second);

                for(auto& path : paths){
                    std::string path_str;
                    for(int i = 1; i<path.size(); ++i) {
                        path_str += dirs[path[i] - path[i-1]];
                    }
                    path_str += 'A';

                #if 0
                    if(num_paths.count({j->second, i->second})){
                    #if 0
                        std::string path_str0 = process_num_pad(path_str, dir_keys, dir_paths);
                        path_str0 = process_num_pad(path_str0, dir_keys, dir_paths);

                        std::string path_str1 = process_num_pad(num_paths[{j->second, i->second}], dir_keys, dir_paths);
                        path_str1 = process_num_pad(path_str1, dir_keys, dir_paths);

                        if(path_str0.size() < path_str1.size()){
                            num_paths[{j->second, i->second}] = path_str;
                        }
                    #endif
                        
                        size_t size0 = process_key_pad_recursive_size(path_str, dir_keys, dir_paths, 0, num_bots, cache_t());
                        size_t size1 = process_key_pad_recursive_size(num_paths[{j->second, i->second}], dir_keys, dir_paths, 0, num_bots, cache_t());

                        if(size0 < size1){
                            num_paths[{j->second, i->second}] = path_str;
                        }


                        //std::cout << "'" << j->first << "': " << j->second << " --> " <<  "'" << i->first << "': " << i->second << " path: " << path_str  << std::endl;
                    }else{
                        num_paths[{j->second, i->second}] = path_str;
                    }

                #endif

                    num_paths[{j->second, i->second}].push_back(path_str);

                    /*num_paths[{j->second, i->second}] = path_str;
                    std::cout << "'" << j->first << "': " << j->second << " --> " <<  "'" << i->first << "': " << i->second << " path: " << path_str  << std::endl;*/
                }
            }


            //num_paths[{j->second, i->second}] = inverse(path_str);

            //std::cout << "'" << i->first << "': " << i->second << " --> " <<  "'" << j->first << "': " << j->second << " path: " << path_str  << std::endl;
            //std::cout << "'" << j->first << "': " << j->second << " --> " <<  "'" << i->first << "': " << i->second << " path: " << inverse(path_str)  << std::endl;

        }
    }

    size_t sum = 0;
    for(auto& code : codes){

        //for(auto& num_path : num_paths){
            std::set<std::string> paths; 
            process_key_pad_many(code, 0, std::string(), num_keys, num_paths, paths);
            //std::string path0;

            size_t len3 = UINT64_MAX;
            for(auto& path : paths){
                //size_t new_len = process_key_pad_recursive_size(path, dir_keys, dir_paths, 0, num_bots, cache_t());
                size_t new_len = process_key_pad_recursive_size_smallest(path, dir_keys, dir_paths_many, 0, num_bots, cache_t());

                //std::cout << new_len << std::endl;
                len3 = std::min(len3, new_len);
            }
            //std::cout << len3 << std::endl;
            //std::cout << std::endl;

            size_t num = std::stoi(code.substr(0,3));
            //std::cout << code << ": " << num << std::endl;
            sum += len3*num;
        //}
    }


    /*
    
                                0               2                           9                         A
                    <           A         ^     A         ^ ^         >     A           v v v         A
          v  < <    A  > >  ^   A   <     A  >  A   <     A A   v  >  A  ^  A   v  <    A A A  >   ^  A
        v<A <A A >>^A vA A ^<A >A v<<A >>^A vA ^A v<<A >>^A A v<A >A ^A <A >A v<A <A >>^A A A vA ^<A >A


                                0               2                           9                         A
                    <           A         ^     A       >           ^ ^     A           v v v         A
          v  < <    A  > >   ^  A    <    A  >  A   v   A    <   ^  A A  >  A    <  v   A A A  >   ^  A
        <vA <A A >>^A vA A <^A >A <v<A >>^A vA ^A <vA >^A <v<A >^A >A A vA ^A <v<A >A >^A A A vA <^A >A



                      3                                     7               9                         A
                ^     A         ^ ^           < <           A       > >     A           v v v         A
           <    A  >  A    <    A A   v  <    A A  > >   ^  A   v   A A  ^  A   v  <    A A A   >  ^  A
        v<<A >>^A vA ^A v<<A >>^A A v<A <A >>^A A vA A ^<A >A v<A >^A A <A >A v<A <A >>^A A A vA ^<A >A 




                      3                                 7               9                         A
                ^     A             < <         ^ ^     A       > >     A           v v v         A  
           <    A  >  A   v  < <    A A  >   ^  A A  >  A   v   A A  ^  A    <  v   A A A  >   ^  A  
        <v<A >>^A vA ^A <vA <A A >>^A A vA <^A >A A vA ^A <vA >^A A <A >A <v<A >A >^A A A vA <^A >A

    */


    /*std::string big_path0;
    big_path0 += num_paths[{num_keys.key_to_pos['A'], num_keys.key_to_pos[codes[0][0]]}];
    for(int i=1; i<codes[0].size(); ++i){
        big_path0 += num_paths[{num_keys.key_to_pos[codes[0][i-1]], num_keys.key_to_pos[codes[0][i]]}];
    }
    std::cout << big_path0 << std::endl;

    std::string big_path1;
    big_path1 += dir_paths[{dir_keys.key_to_pos['A'], dir_keys.key_to_pos[big_path0[0]]}];
    for(int i=1; i<big_path0.size(); ++i){
        big_path1 += dir_paths[{dir_keys.key_to_pos[big_path0[i-1]], dir_keys.key_to_pos[big_path0[i]]}];
    }
    std::cout << big_path1 << std::endl;

    std::string big_path2;
    big_path2 += dir_paths[{dir_keys.key_to_pos['A'], dir_keys.key_to_pos[big_path1[0]]}];
    for(int i=1; i<big_path1.size(); ++i){
        big_path2 += dir_paths[{dir_keys.key_to_pos[big_path1[i-1]], dir_keys.key_to_pos[big_path1[i]]}];
    }
    std::cout << big_path2 << std::endl;

    std::cout << std::stoi(codes[0].substr(0,3)) << std::endl;
    std::cout << big_path2.size() << std::endl;
    std::cout << (std::stoi(codes[0].substr(0,3)) * big_path2.size()) << std::endl;*/

    return sum;
}

int part2(const codes_t& codes)
{
    return 0;
}

void main()
{
    auto test_values = load_input("../src/day21/test_input.txt");
    auto actual_values = load_input("../src/day21/input.txt");

    std::cout << "part1: " << part1(test_values, 2) << std::endl;
    std::cout << "part1: " << part1(actual_values, 2) << std::endl;

    std::cout << "part2: " << part1(test_values, 25) << std::endl;
    std::cout << "part2: " << part1(actual_values, 25) << std::endl;

    // 166048 too high
    // 161014362301504 too low
    // 196910339808654 is correct
    // 413503376430618 too high

    //std::cout << "part2: " << part2(test_values) << std::endl;
    //std::cout << "part2: " << part2(actual_values) << std::endl;
}