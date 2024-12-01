#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

struct list_t{
    std::vector<int> a;
    std::vector<int> b;
};

list_t load_input(const std::string& file){
    list_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        std::istringstream line_stream(line);
        int num1, num2;
        line_stream >> num1 >> num2;
        ret.a.push_back(num1);
        ret.b.push_back(num2);
    }
    return ret;
}

int part1(list_t list)
{
    std::sort(list.a.begin(), list.a.end());
    std::sort(list.b.begin(), list.b.end());

    int sum = 0;
    for(int i=0; i<list.a.size(); ++i){
        sum += std::abs(list.a[i] - list.b[i]);
    }

    return sum;
}

int part2(const list_t& list)
{
    std::map<int, int> histogram;
    for(int i=0; i<list.a.size(); ++i){
        histogram[list.b[i]]++;
    }

    int sum = 0;
    for(int i=0; i<list.a.size(); ++i){
        sum += list.a[i] * histogram[list.a[i]];
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day01/test_input.txt");
    auto actual_values = load_input("../src/day01/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}