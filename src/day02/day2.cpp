#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>

using report_t = std::vector<int>;
using reports_t = std::vector<report_t>;

reports_t load_input(const std::string& file){
    reports_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(report_t());
        int number;
        std::istringstream line_stream(line);
        while (line_stream >> number) {
            ret.back().push_back(number);
        }
    }
    return ret;
}

bool is_safe(const report_t& report){
    report_t diffs;
    std::adjacent_difference(report.begin(), report.end(), std::back_inserter(diffs), std::minus<>{}); // get neighbouring element differences

    return std::all_of(diffs.begin()+1, diffs.end(), [&](auto d){  
        return std::abs(d) >= 1 &&  // diff >= 1
               std::abs(d) <= 3 &&  // diff <= 3
               (d ^ diffs[1]) >= 0; // same sign
    });
}

bool is_safe_subreport(const report_t& report)
{
    // iterate over all reports that have 1 element removed (all n-1 combinations)
    std::vector<int> bitset(report.size()-1, 1);
    bitset.resize(report.size(), 0);
 
    do {
        report_t sub_report;
        for (size_t i=0; i<report.size(); ++i) {
            if(bitset[i]) {
                sub_report.push_back(report[i]);
            }
        }
        if(is_safe(sub_report)){
            return true;
        }
    } 
    while (std::prev_permutation(bitset.begin(), bitset.end()));

    return false;
}

int part1(const reports_t& reports)
{
    return std::accumulate(reports.begin(), reports.end(), 0, [](int safe_count, const report_t& report){ 
        return safe_count + is_safe(report); 
    });
}

int part2(const reports_t& reports)
{
    return std::accumulate(reports.begin(), reports.end(), 0, [](int safe_count, const report_t& report){ 
        return safe_count + (is_safe(report) || is_safe_subreport(report)); 
    });
}

void main()
{
    auto test_values = load_input("../src/day02/test_input.txt");
    auto actual_values = load_input("../src/day02/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}