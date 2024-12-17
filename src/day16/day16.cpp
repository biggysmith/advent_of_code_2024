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
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t rotate(const pos_t& dir, bool ccw){ return ccw ? pos_t{ dir.y, -dir.x } : pos_t{ -dir.y, dir.x }; }

struct map_t{
    std::vector<char> data;
    pos_t start, end;
    int width = 0;
    int height = 0;

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
    int path_id;
};

bool operator<(const step_t& a, const step_t& b){ return a.score > b.score; }
struct big_int { int i = INT_MAX; }; // want default dist value to be MAX for initial comparison 

auto process(const map_t& map) 
{
    std::priority_queue<step_t> q;
    q.push({map.start, {1,0}, 0, 0}); 

    std::map<pos_t, std::map<pos_t, big_int>> dist;  
    dist[map.start][{1,0}].i = 0;

    std::set<pos_t> tile_set;

    std::vector<std::vector<pos_t>> paths;
    paths.push_back(std::vector<pos_t>());

    int min_score = -1;

    while (!q.empty()) {
        auto curr = q.top();
        q.pop();

        paths[curr.path_id].push_back(curr.pos);

        if (curr.pos == map.end) {       
            if (min_score == -1) {
                min_score = curr.score;
            }
            if(curr.score == min_score){
                tile_set.insert(paths[curr.path_id].begin(), paths[curr.path_id].end());
            }
            continue;
        }

        for (auto& new_dir : { curr.dir, rotate(curr.dir, true), rotate(curr.dir, false) }) {
            pos_t new_pos = curr.pos + new_dir;
            int new_score = curr.score + (new_dir == curr.dir ? 1 : 1001);

            if (map.in_grid(new_pos) && map.get(new_pos) == '.' && new_score <= dist[new_pos][new_dir].i) {
                if(new_dir == curr.dir){
                    q.push({new_pos, new_dir, new_score, curr.path_id});
                }else{
                    q.push({new_pos, new_dir, new_score, (int)paths.size()});
                    paths.push_back(paths[curr.path_id]);
                }
                dist[new_pos][new_dir].i = new_score;
            }
        }
    }

    return std::make_pair(min_score, tile_set.size());
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
}