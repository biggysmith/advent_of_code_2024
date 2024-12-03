#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <regex>

std::string load_input(const std::string& file){
    std::string ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret += line;
    }
    return ret;
}

int mul(const std::string& str){
    int number0, number1;
    sscanf_s(str.c_str(), "mul(%d,%d)", &number0, &number1);
    return number0 * number1;
}

int part1(const std::string& section)
{
    std::regex r("(mul\\([0-9]{1,3},[0-9]{1,3}\\))");

    int sum = 0;
    for (std::sregex_iterator i = std::sregex_iterator(section.begin(), section.end(), r); i != std::sregex_iterator(); ++i) {
        sum += mul(i->str());
    }

    return sum;
}

int part2(const std::string& section)
{
    std::regex r("(do\\(\\)|don't\\(\\)|mul\\([0-9]{1,3},[0-9]{1,3}\\))");

    int sum = 0;
    bool enabled = true;

    for (std::sregex_iterator i = std::sregex_iterator(section.begin(), section.end(), r); i != std::sregex_iterator(); ++i) {
        if(i->str() == "do()"){
            enabled = true;
        }else if(i->str() == "don't()"){
            enabled = false;
        }else if(enabled){
            sum += mul(i->str());
        }
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day03/test_input.txt");
    auto test_values2 = load_input("../src/day03/test_input2.txt");
    auto actual_values = load_input("../src/day03/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values2) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}