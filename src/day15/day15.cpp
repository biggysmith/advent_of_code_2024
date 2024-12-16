#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a, const pos_t& b){ return std::tie(a.x, a.y) == std::tie(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }

struct map_t{
    std::vector<char> data;
    std::vector<char> moves;
    pos_t bot_pos;
    int width = 0;
    int height = 0;

    char& get(const pos_t& p) { return data[p.y * width + p.x]; }
    char get(const pos_t& p) const { return data[p.y * width + p.x]; }
};

map_t load_input(const std::string& file){
    map_t ret;
    int mode = 0;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        if(line.empty()){
            mode = 1;  
        }
        for(int x=0; x<line.size(); ++x){
            if(line[x]=='@'){
                ret.bot_pos = {x, ret.height};
                ret.data.push_back('.');
            }else{
                (mode==0 ? ret.data : ret.moves).push_back(line[x]);
            }
        }
        if(mode==0){
            ret.width = std::max(ret.width, (int)line.size());
            ret.height++;
        }
    }
    return ret;
}

std::map<char, pos_t> dirs { {'>', {1,0}}, {'v', {0,1}}, {'<', {-1,0}}, {'^', {0,-1}} };

int gps(const map_t& map, char box){
    int sum = 0;
    for(int y=0; y<map.height; ++y){
        for(int x=0; x<map.width; ++x){
            if(map.get({x,y}) == box){
                sum += 100*y + x;
            }
        }
    }
    return sum;
}

bool try_move(map_t& map, const pos_t& curr_pos, const pos_t& next_pos, const pos_t& dir, bool move=false)
{
    if(map.get(next_pos) == '[' || map.get(next_pos) == ']' || map.get(next_pos) == 'O') // hit a box
    {
        if(map.get(next_pos) != 'O' && (dir == dirs['^'] || dir == dirs['v'])) // pushing wide boxes up or down
        {        
            if(map.get(next_pos) == '[') {
                if(!try_move(map, next_pos, next_pos+dir, dir, move)) { return false; } // try move [
                if(!try_move(map, next_pos+dirs['>'], (next_pos+dirs['>'])+dir, dir, move)) { return false; } // try move ]
            }

            if(map.get(next_pos) == ']') {
                if(!try_move(map, next_pos, next_pos+dir, dir, move)) { return false; } // try move ]
                if(!try_move(map, next_pos+dirs['<'], (next_pos+dirs['<'])+dir, dir, move)) { return false; } // try move [
            }
        }
        else if(!try_move(map, next_pos, next_pos+dir, dir, move)){ // pushing wide box left or right, or regular box in any direction
            return false;
        }
    }

    if(map.get(next_pos) == '#'){ // can't move
        return false;
    }else if(map.get(next_pos) == '.'){ // can move
        if(move){ 
            std::swap(map.get(curr_pos), map.get(next_pos));
        }
        return true;
    }
    return true; // can potentially move (e.g. mid box)
}

void do_move(map_t& map, const pos_t& curr_pos, const pos_t& next_pos, const pos_t& dir){
    try_move(map, curr_pos, next_pos, dir, true);
}

map_t expand_map(const map_t& map){
    map_t big_map;
    big_map.width = map.width*2;
    big_map.height = map.height;
    big_map.data.resize(map.width*2 * map.height);
    big_map.moves = map.moves;

    for(int y=0; y<map.height; ++y){
        for(int x=0; x<map.width; ++x){
            if(map.get({x,y}) == '#'){
                big_map.get({x*2+0,y}) = '#';
                big_map.get({x*2+1,y}) = '#';
            }else if(map.get({x,y}) == 'O'){
                big_map.get({x*2+0,y}) = '[';
                big_map.get({x*2+1,y}) = ']';
            }else if(map.get({x,y}) == '.'){
                big_map.get({x*2+0,y}) = '.';
                big_map.get({x*2+1,y}) = '.';
            }
            if(map.bot_pos == pos_t{x,y}){
                big_map.bot_pos = {x*2,y};
            }
        }
    }
    return big_map;
}

int process(const map_t& in_map, bool expand)
{
    map_t map = expand ? expand_map(in_map) : in_map;

    pos_t pos = map.bot_pos;
    for(auto move : map.moves){
        pos_t next_pos = pos + dirs[move];
        if(try_move(map, pos, next_pos, dirs[move])){
            do_move(map, pos, next_pos, dirs[move]);
            pos = next_pos;
        };
    }

    return gps(map, expand ? '[' : 'O');
}

void main()
{
    auto test_values0 = load_input("../src/day15/test_input0.txt");
    auto test_values1 = load_input("../src/day15/test_input1.txt");
    auto test_values2 = load_input("../src/day15/test_input2.txt");
    auto actual_values = load_input("../src/day15/input.txt");

    std::cout << "part1: " << process(test_values0, false) << std::endl;
    std::cout << "part1: " << process(test_values1, false) << std::endl;
    std::cout << "part1: " << process(actual_values, false) << std::endl;

    std::cout << "part2: " << process(test_values1, true) << std::endl;
    std::cout << "part2: " << process(actual_values, true) << std::endl;
}