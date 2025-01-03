#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <numeric>

using wires_t = std::unordered_map<std::string, bool>;

struct gate_t {
    std::string in0, op, in1, out;

    bool is_in_xy() const { return (in0[0] == 'x' || in0[0] == 'y') && (in1[0] == 'x' || in1[0] == 'y'); };
    bool is_in_00() const { return in0.substr(1) == "00" && in1.substr(1) == "00"; };
};

struct circuit_t {
    wires_t state;
    std::vector<gate_t> gates;
    int zcount = 0;
};

circuit_t load_input(const std::string& file){
    circuit_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line) && !line.empty()) {
        ret.state[line.substr(0, 3)] = std::stoi(line.substr(5, 1));
    }
    while (std::getline(fs, line)) {
        ret.gates.push_back(gate_t());
        std::istringstream iss(line); std::string temp;
        iss >> ret.gates.back().in0 >> ret.gates.back().op >> ret.gates.back().in1 >> temp >> ret.gates.back().out;
        if(ret.gates.back().out[0] == 'z'){ ret.zcount++; }
    }
    return ret;
}

uint64_t part1(circuit_t circuit)
{
    uint64_t z_seen = 0;
    uint64_t z_all = (1ull << circuit.zcount) - 1;

    while(z_seen != z_all)
    {
        for(auto& gate : circuit.gates)
        {
            if(circuit.state.count(gate.in0) && circuit.state.count(gate.in1) && z_seen != z_all)
            {
                if(gate.op == "AND"){
                    circuit.state[gate.out] = circuit.state[gate.in0] & circuit.state[gate.in1];
                }else if(gate.op == "OR"){
                    circuit.state[gate.out] = circuit.state[gate.in0] | circuit.state[gate.in1];
                }else if(gate.op == "XOR"){
                    circuit.state[gate.out] = circuit.state[gate.in0] ^ circuit.state[gate.in1];
                }

                if(gate.out[0] == 'z'){
                    int zi = (gate.out[1] - '0') * 10 + (gate.out[2] - '0');
                    z_seen |= 1ull << zi;
                }
            }
        }
    }

    uint64_t binary = 0;
    for(int z=0; z<circuit.zcount; ++z){
        if(circuit.state[(z < 10 ? "z0" : "z") + std::to_string(z)]){
            binary |= 1ull << z;
        }
    }

    return binary;
}

auto part2(const circuit_t& circuit)
{
    std::set<std::string> wires;
    std::string zlast = "z" + std::to_string(circuit.zcount-1);

    for(auto& gate : circuit.gates)
    {
        if(gate.out[0]=='z' && gate.op != "XOR" && gate.out.substr(0) != zlast){
            wires.insert(gate.out); // both (a or b -> z), (a and b -> z) breaks circuit
        }

        if(gate.out[0]!='z' && !gate.is_in_xy() && gate.op == "XOR"){
            wires.insert(gate.out); // non-xy xor non-xy -> non-z breaks circuit
        }

        if (gate.op == "XOR" && gate.is_in_xy() && !gate.is_in_00()) {
            if(bool not_in_later_xor_gate = std::none_of(circuit.gates.begin(), circuit.gates.end(), [&](auto& g) { return (g.in0 == gate.out || g.in1 == gate.out) && g.op == "XOR"; })){
                wires.insert(gate.out); // (xy xor xy -> out) then !(out xor b -> c) or !(a xor out -> c) breaks circuit
            }
        }

        if (gate.op == "AND" && gate.is_in_xy() && !gate.is_in_00()) {
            if(bool not_in_later_or_gate = std::none_of(circuit.gates.begin(), circuit.gates.end(), [&](auto& g) { return (g.in0 == gate.out || g.in1 == gate.out) && g.op == "OR"; })){
                wires.insert(gate.out); // (xy and xy -> out) then !(out or b -> c) or !(a or out -> c) breaks circuit
            }
        }
    }

    return std::accumulate(std::next(wires.begin()), wires.end(), *wires.begin(), [](const std::string& a, const std::string& b) { 
        return a + "," + b; 
    });
}

void main()
{
    auto test_values0 = load_input("../src/day24/test_input0.txt");
    auto test_values1 = load_input("../src/day24/test_input1.txt");
    auto actual_values = load_input("../src/day24/input.txt");

    std::cout << "part1: " << part1(test_values0) << std::endl;
    std::cout << "part1: " << part1(test_values1) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(actual_values) << std::endl;
}