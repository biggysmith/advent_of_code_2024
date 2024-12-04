#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

struct grid_t{
    std::string letters;
    int width;
    int height;

    char get(int x, int y) const{
        return letters[y*width+x];
    }
};

grid_t load_input(const std::string& file){
    grid_t ret;
    std::ifstream fs(file);
    std::string line;
    ret.width = 0;
    ret.height = 0;
    while (std::getline(fs, line)) {
        ret.letters += line;
        ret.width = std::max(ret.width, (int)line.size());
        ret.height++;
    }
    return ret;
}

bool is_xmas(char a, char b, char c, char d){
    return (a=='X' && b=='M' && c=='A' && d=='S') || (a=='S' && b=='A' && c=='M' && d=='X');
}

bool is_x_mas(char tl, char tr, char c, char bl, char br){
    return ((tl=='M' && c=='A' && br=='S') || (tl=='S' && c=='A' && br=='M')) && ((tr=='M' && c=='A' && bl=='S') || (tr=='S' && c=='A' && bl=='M'));
}

int part1(const grid_t& grid)
{
    int matches = 0;

    // search horizontal
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<=(grid.width-4); ++x){
            matches += is_xmas(grid.get(x,y), grid.get(x+1,y), grid.get(x+2,y), grid.get(x+3,y));
        }
    }

    // search vertical
    for(int x=0; x<grid.width; ++x){
        for(int y=0; y<=(grid.height-4); ++y){
            matches += is_xmas(grid.get(x,y), grid.get(x,y+1), grid.get(x,y+2), grid.get(x,y+3));
        }
    }

    // search diagonal right to left
    for(int x=3; x<grid.width; ++x){
        for(int y=0; y<=(grid.height-4); ++y){
            matches += is_xmas(grid.get(x,y), grid.get(x-1,y+1), grid.get(x-2,y+2), grid.get(x-3,y+3));
        }
    }

    // search diagonal left to right
    for(int x=0; x<=(grid.width-4); ++x){
        for(int y=0; y<=(grid.height-4); ++y){
            matches += is_xmas(grid.get(x,y), grid.get(x+1,y+1), grid.get(x+2,y+2), grid.get(x+3,y+3));
        }
    }

    return matches;
}

int part2(const grid_t& grid)
{
    int matches = 0;

    for(int x=1; x<grid.width-1; ++x){
        for(int y=1; y<grid.height-1; ++y){
            matches += is_x_mas(grid.get(x-1,y-1), grid.get(x+1,y-1), grid.get(x,y), grid.get(x-1,y+1), grid.get(x+1,y+1));
        }
    }

    return matches;
}

void main()
{
    auto test_values = load_input("../src/day04/test_input.txt");
    auto actual_values = load_input("../src/day04/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}