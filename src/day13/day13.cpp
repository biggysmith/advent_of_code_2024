#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <matrix.hpp>

struct machine_t{
    uint64_t ax, ay, bx, by, px, py;
};
using machines_t = std::vector<machine_t>;

machines_t load_input(const std::string& file){
    machines_t ret;
    ret.push_back(machine_t());
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        if(line.empty()) {
            ret.push_back(machine_t());
        }else if(line.find("Button A") != std::string::npos){
            sscanf_s(line.c_str(), "Button A: X+%llu, Y+%llu", &ret.back().ax, &ret.back().ay);
        }else if(line.find("Button B") != std::string::npos){
            sscanf_s(line.c_str(), "Button B: X+%llu, Y+%llu", &ret.back().bx, &ret.back().by);
        }else if(line.find("Prize") != std::string::npos){
            sscanf_s(line.c_str(), "Prize: X=%llu, Y=%llu", &ret.back().px, &ret.back().py);
        }
    }
    return ret;
}

bool whole_num(double num) { return std::fabs(num - std::round(num)) < 1e-3; }
bool valid(double num) { return num >= 0 && whole_num(num); }
bool valid(const dvec2& v) { return valid(v.x) && valid(v.y); }

uint64_t process(const machines_t& machines, uint64_t offset)
{
    double sum = 0;

    for(auto& machine : machines){
        dmat22 mat(
            (double)machine.ax, (double)machine.bx,
            (double)machine.ay, (double)machine.by
        );

        dvec2 vec(
            (double)(machine.px + offset), 
            (double)(machine.py + offset)
        );

        vec = inverse(mat) * vec;
        if(valid(vec)){
            sum += std::round(vec.x)*3 + std::round(vec.y);
        }
    }

    return (uint64_t)sum;
}

void main()
{
    auto test_values = load_input("../src/day13/test_input.txt");
    auto actual_values = load_input("../src/day13/input.txt");

    std::cout << "part1: " << process(test_values, 0) << std::endl;
    std::cout << "part1: " << process(actual_values, 0) << std::endl;

    std::cout << "part2: " << process(test_values, 10'000'000'000'000) << std::endl;
    std::cout << "part2: " << process(actual_values, 10'000'000'000'000) << std::endl;
}