#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <array>
#include <numeric>
#include <set>
#include <unordered_set>
#include <unordered_map>

using computer_t = int;
using set_t = std::unordered_set<computer_t>;
using graph_t = std::unordered_map<computer_t, set_t>;

int to_int(const std::string& s){ return (s[0]-'a')*26 + (s[1]-'a'); } // encode ab string to yx grid
std::string to_str(int i){ return std::string(1, 'a' + (i / 26)) + std::string(1, 'a' + (i % 26)); } // convert yx num back to ab string

graph_t load_input(const std::string& file){
    graph_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret[to_int(line.substr(0,2))].insert(to_int(line.substr(3,2)));
        ret[to_int(line.substr(3,2))].insert(to_int(line.substr(0,2)));
    }
    return ret;
}

template<typename Seq>
bool any_start_with_t(const Seq& seq){
    return std::any_of(seq.begin(), seq.end(), [](const computer_t& com) { return (com / 26) == 19; });
}

int part1(const graph_t& graph)
{
    int sum = 0;

    for(auto i=graph.begin(); i!=graph.end(); ++i) {
        auto& [com_a, links_a] = *i;
        for(auto j=std::next(i); j!=graph.end(); ++j) {
            auto& [com_b, links_b] = *j;

            if(!links_a.count(com_b)) {
                continue;
            }

            for(auto k=std::next(j); k!=graph.end(); ++k) {
                auto& [com_c, _] = *k;
                if(links_a.count(com_c) && links_b.count(com_c)) {
                    if(any_start_with_t(std::array{com_a, com_b, com_c})){
                        sum++;
                    }
                }
            }
        }
    }

    return sum;
}

// https://en.wikipedia.org/wiki/Bron-Kerbosch_algorithm

void bron_kerbosch(const graph_t& graph, set_t& r, set_t& p, set_t& x, set_t& largest_clique) {
    if(p.empty() && x.empty()) {
        if(any_start_with_t(r)){
            if(r.size() > largest_clique.size()){
                largest_clique = r;
            }
        }
        return;
    }

    computer_t pivot = p.empty() ? *x.begin() : *p.begin();

    set_t non_linked;
    for(auto& v : p) {
        if(!graph.at(pivot).count(v)) {
            non_linked.insert(v);
        }
    }

    for(auto& v : non_linked) {
        r.insert(v);

        set_t new_p, new_x;
        for(auto& u : p) {
            if(graph.at(v).count(u)) {
                new_p.insert(u);
            }
        }

        for(auto& u : x) {
            if(graph.at(v).count(u)) {
                new_x.insert(u);
            }
        }

        bron_kerbosch(graph, r, new_p, new_x, largest_clique);

        r.erase(v);
        p.erase(v);
        x.insert(v);
    }
}

auto find_largest_clique(const graph_t& graph) {
    set_t r, p, x, clique;
    for (auto& [com, _] : graph) { p.insert(com); }
    bron_kerbosch(graph, r, p, x, clique);
    return std::set<computer_t>(clique.begin(), clique.end()); // need it in sorted order
}

std::string part2(const graph_t& graph)
{
    auto clique = find_largest_clique(graph);

    return std::accumulate(std::next(clique.begin()), clique.end(), to_str(*clique.begin()), [](const std::string& a, const computer_t& b) { 
        return a + "," + to_str(b); 
    });
}

void main()
{
    auto test_values = load_input("../src/day23/test_input.txt");
    auto actual_values = load_input("../src/day23/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}