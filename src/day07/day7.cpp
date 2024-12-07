#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

struct equation_t{
    uint64_t val;
    std::vector<uint64_t> nums;
};

using equations_t = std::vector<equation_t>;

equations_t load_input(const std::string& file){
    equations_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        uint64_t val = std::stoll(line.substr(0, line.find(':')));
        ret.push_back(equation_t());
        ret.back().val = val;
        std::stringstream ss(line.substr(line.find(':')+2));
        std::string str;
        while (std::getline(ss, str, ' ')) {
            ret.back().nums.push_back(std::stoll(str));
        }
    }
    return ret;
}

uint64_t concatenate(uint64_t a, uint64_t b) {
    uint64_t pow = 10;
    while(b >= pow) pow *= 10;
    return a * pow + b;
}

template<bool Concat, typename First, typename Last>
bool evaluate(uint64_t target, uint64_t current, First first, Last last){
    if(first != last){
        return evaluate<Concat>(target, current + (*first), first+1, last) || // addition +
               evaluate<Concat>(target, current * (*first), first+1, last) || // multiplication *
               (Concat ? evaluate<Concat>(target, concatenate(current, *first), first+1, last) : false); // concatenate ||
    }else{
        return target == current;
    }
}

uint64_t part1(const equations_t& equations)
{
    uint64_t sum = 0;
    for(auto& eq : equations){
        if(evaluate<false>(eq.val, eq.nums.front(), eq.nums.begin()+1, eq.nums.end())){
            sum += eq.val;
        }
    }
    return sum;
}

uint64_t part2(const equations_t& equations)
{
    uint64_t sum = 0;
    for(auto& eq : equations){
        if(evaluate<true>(eq.val, eq.nums.front(), eq.nums.begin()+1, eq.nums.end())){
            sum += eq.val;
        }
    }
    return sum;
}

void main()
{
    auto test_values = load_input("../src/day07/test_input.txt");
    auto actual_values = load_input("../src/day07/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}