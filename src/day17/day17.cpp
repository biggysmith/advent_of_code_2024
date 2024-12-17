#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <optional>

struct program_t{
    std::vector<uint64_t> reg;
    std::vector<int> prog;
    std::vector<int> output;
    int ptr = 0;
};

program_t load_input(const std::string& file){
    program_t ret;
    ret.reg.resize(3);
    std::ifstream fs(file);
    std::string line;
    std::getline(fs, line);
    sscanf_s(line.c_str(), "Register A: %lld", &ret.reg[0]);
    std::getline(fs, line);
    sscanf_s(line.c_str(), "Register B: %lld", &ret.reg[1]);
    std::getline(fs, line);
    sscanf_s(line.c_str(), "Register C: %lld", &ret.reg[2]);
    std::getline(fs, line);
    std::getline(fs, line);
    std::string sub = line.substr(9);
    std::istringstream iss(sub);
    std::string token;
    while (std::getline(iss, token, ',')) {
        ret.prog.push_back(std::stoi(token)); 
    }
    return ret;
}

constexpr int A=0, B=1, C=2, adv=0, bxl=1, bst=2, jnz=3, bxc=4, out=5, bdv=6, cdv=7;

uint64_t combo(const program_t& p, int operand){
    if(operand <= 3){
        return operand;
    }else if(operand == 4){
        return p.reg[A];
    }else if(operand == 5){
        return p.reg[B];
    }else if(operand == 6){
        return p.reg[C];
    }else /*if(operand > 6)*/{
        return operand;
    }
}

auto run(program_t p)
{
    while(p.ptr < p.prog.size())
    {
        int opcode = p.prog[p.ptr];
        int operand = p.prog[p.ptr+1];

        if(opcode == adv){
            p.reg[A] >>= combo(p, operand);
        }else if(opcode == bxl){
            p.reg[B] ^= operand;
        }else if(opcode == bst){
            p.reg[B] = combo(p, operand) % 8;
        }else if(opcode == jnz && p.reg[A]){
            p.ptr = operand;
            continue;
        }else if(opcode == bxc){
            p.reg[B] ^= p.reg[C];
        }else if(opcode == out){
            p.output.push_back(combo(p, operand) % 8);
        }else if(opcode == bdv){
            p.reg[B] = p.reg[A] >> combo(p, operand);
        }else if(opcode == cdv){
            p.reg[C] = p.reg[A] >> combo(p, operand);
        }

        p.ptr += 2;
    }
    return p.output;
}

std::string part1(const program_t& program)
{
    std::vector<int> output = run(program);
    return std::accumulate(output.begin()+1, output.end(), std::to_string(output.front()), [](const std::string& a, int b) { 
        return a + "," + std::to_string(b); 
    });
}

std::optional<uint64_t> search(const program_t& program, uint64_t reg_a, size_t depth) 
{
    if (depth < 0) {
        return std::nullopt;
    }

    for(uint64_t j=0; j<8; ++j) {
        program_t new_program = program;
        new_program.reg[A] = reg_a | (j << (3 * depth)); // we wanna vary 3-bit chunks in reg at each search level
        std::vector<int> output = run(new_program);

        if(output.size() == program.prog.size()) {
            if(output == program.prog) {
                return new_program.reg[A];
            }
            if(output[depth] == program.prog[depth]) {
                auto result = search(program, new_program.reg[A], depth-1);
                if(result) {
                    return result; 
                }
            }
        }
    }

    return std::nullopt;
}

uint64_t part2(const program_t& p)
{    
    return *search(p, 0ull, p.prog.size()-1);
}

void main()
{
    auto test_values0 = load_input("../src/day17/test_input0.txt");
    auto test_values1 = load_input("../src/day17/test_input1.txt");
    auto actual_values = load_input("../src/day17/input.txt");

    std::cout << "part1: " << part1(test_values0) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values1) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}