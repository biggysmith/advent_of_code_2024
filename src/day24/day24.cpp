#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>

#include <timer.hpp>
#include <unordered_map>
#include <numeric>

#define AND 0
#define OR 1
#define XOR 2
#define Z 35
#define Y 34
#define X 33

int z_idx(int i) { return i / (36 * 36); };
int y_idx(int i) { return (i / 36) % 36; };
int x_idx(int i) { return i % 36; };

struct hasher {
    std::size_t operator()(const std::string& s) const {
        return s[0]*36*36 + s[1]*36 + s[2];
    }
};

using wire_t = std::string;
using wires_t = std::unordered_map<wire_t, int>;

struct gate_t {
    int opi;
    std::string op;
    wire_t in0, in1, out;

    int i_in0, i_in1, i_out;
};

struct circuit_t{
    wires_t state;
    std::vector<gate_t> gates;
    int zcount = 0;
    int ycount = 0;
    int xcount = 0;

    std::vector<int> statei = std::vector<int>(36*36*36, -1);

    std::vector<int> opsi;
    std::vector<int> gatesi;

    std::vector<int> in0_data;
    std::vector<int> in1_data;
    std::vector<int> out_data;


    //int& in0(int i) { return statei[gatesi[i]]; };
    //int& in1(int i) { return statei[gatesi[i+1]]; };
    //int& out(int i) { return statei[gatesi[i+2]]; };

    int& in0(int i) { return statei[in0_data[i]]; };
    int& in1(int i) { return statei[in1_data[i]]; };
    int& out(int i) { return statei[out_data[i]]; };
};

uint32_t base36Hash(const std::string& str) {
    auto charToIndex = [](char c) -> uint32_t {
        if (c >= '0' && c <= '9') return c - '0';          // Map '0'-'9' to 0-9
        else/*if (c >= 'a' && c <= 'z')*/ return c - 'a' + 10;     // Map 'a'-'z' to 10-35
        //throw std::invalid_argument("Invalid character in string.");
        //__assume(false);
    };

    return charToIndex(str[0]) * 36 * 36 +
           charToIndex(str[1]) * 36 +
           charToIndex(str[2]);
}



std::string inv_base36Hash(uint32_t i) {
    int z = z_idx(i);
    int y = y_idx(i);
    int x = x_idx(i);

    std::string ret(3, '0');
    if(z>9 && z<36){
        ret[0] = (z-10) + 'a';
    }else{
        ret[0] = z + '0';
    }

    if(y>9 && y<36){
        ret[1] = (y-10) + 'a';
    }else{
        ret[1] = y + '0';
    }

    if(x>9 && x<36){
        ret[2] = (x-10) + 'a';
    }else{
        ret[2] = x + '0';
    }

    return ret;
}



/*
uint32_t base36Hash_z(int z) {
    int xx = z + 'a' + 10;
    int yy = z + '0' / 10;
    int zz = z % 10;
    return xx*36*36 + yy*36 + xx;
}*/

circuit_t load_input(const std::string& file){
    circuit_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line) && !line.empty()) {
        ret.state[line.substr(0, 3)] = std::stoi(line.substr(5, 1));
        ret.statei[base36Hash(line.substr(0, 3))] = std::stoi(line.substr(5, 1));
        if(line.substr(0, 3)[0] == 'y'){ ret.ycount++; }
        if(line.substr(0, 3)[0] == 'x'){ ret.xcount++; }

    }
    while (std::getline(fs, line)) {
        ret.gates.push_back(gate_t());
        std::istringstream iss(line); std::string temp;
        iss >> ret.gates.back().in0 >> ret.gates.back().op >> ret.gates.back().in1 >> temp >> ret.gates.back().out;
        if(ret.gates.back().out[0] == 'z'){ ret.zcount++; }
        /*if(ret.gates.back().out[0] == 'y'){ ret.ycount++; }
        if(ret.gates.back().out[0] == 'x'){ ret.xcount++; }*/
        if(ret.gates.back().op == "AND"){ ret.gates.back().opi = AND; }
        else if(ret.gates.back().op == "OR"){ ret.gates.back().opi = OR; }
        else if(ret.gates.back().op == "XOR"){ ret.gates.back().opi = XOR; }

        ret.gates.back().i_in0 = base36Hash(ret.gates.back().in0);
        ret.gates.back().i_in1 = base36Hash(ret.gates.back().in1);
        ret.gates.back().i_out = base36Hash(ret.gates.back().out);

        ret.opsi.push_back(ret.gates.back().opi);
        ret.gatesi.push_back(ret.gates.back().i_in0);
        ret.gatesi.push_back(ret.gates.back().i_in1);
        ret.gatesi.push_back(ret.gates.back().i_out);

        ret.in0_data.push_back(ret.gates.back().i_in0);
        ret.in1_data.push_back(ret.gates.back().i_in1);
        ret.out_data.push_back(ret.gates.back().i_out);
    }
    return ret;
}

/*
uint64_t part1(circuit_t circuit)
{
    //std::cout << std::endl;
    scoped_timer t(e_microseconds); 

    uint64_t z_set = 0;
    uint64_t z_max = (1ull << circuit.zcount) - 1;
    while(z_set != z_max)
    {
        for(auto& gate : circuit.gates){
            if(circuit.state.count(gate.in0) && circuit.state.count(gate.in1) && z_set != z_max)
            {
                if(gate.opi == AND){
                    circuit.state[gate.out] = circuit.state[gate.in0] & circuit.state[gate.in1];
                    //std::cout << gate.in0 << " & " << gate.in1 << " -> " << gate.out << std::endl;
                }else if(gate.opi == OR){
                    circuit.state[gate.out] = circuit.state[gate.in0] | circuit.state[gate.in1];
                    //std::cout << gate.in0 << " | " << gate.in1 << " -> " << gate.out << std::endl;
                }else if(gate.opi == XOR){
                    circuit.state[gate.out] = circuit.state[gate.in0] ^ circuit.state[gate.in1];
                    //std::cout << gate.in0 << " ^ " << gate.in1 << " -> " << gate.out << std::endl;
                }

                if(gate.out[0] == 'z'){
                    int zi = std::stoi(gate.out.substr(1));
                    //int zi = gate.out[0]-'0';
                    z_set |= 1ull << zi;
                }
            }
        }
    }

    //std::cout << "op_count: " << op_count << std::endl;

    int i = 0;
    uint64_t binary = 0;
    for(auto it=circuit.state.find("z00"); it!=circuit.state.end(); ++it, ++i){
        if(it->second == 1) { binary |= 1ull << i; }
    }

    return binary;
}
*/



auto process(circuit_t circuit)
{
    //std::cout << std::endl;
    //scoped_timer t(e_microseconds); 

    uint64_t y_binary = 0;
    for(int y=0; y<circuit.ycount; ++y){
        if(circuit.state[(y < 10 ? "y0" : "y") + std::to_string(y)] == 1){
            y_binary |= 1ull << y;
        }
    }

    uint64_t x_binary = 0;
    for(int x=0; x<circuit.xcount; ++x){
        if(circuit.state[(x < 10 ? "x0" : "x") + std::to_string(x)] == 1){
            x_binary |= 1ull << x;
        }
    }

    int runs = 0;
    int max_runs = (circuit.zcount+1)*2;

    int op_count = 0;
    uint64_t z_set = 0;
    uint64_t z_max = (1ull << circuit.zcount) - 1;

    while(z_set != z_max && runs < max_runs)
    {
        runs++;

        for(auto& gate : circuit.gates){
            if(circuit.state.count(gate.in0) && circuit.state.count(gate.in1) && z_set != z_max)
            {
                if(gate.opi == AND){
                    circuit.state[gate.out] = circuit.state[gate.in0] & circuit.state[gate.in1];
                    //std::cout << gate.in0 << " & " << gate.in1 << " -> " << gate.out << std::endl;
                }else if(gate.opi == OR){
                    circuit.state[gate.out] = circuit.state[gate.in0] | circuit.state[gate.in1];
                    //std::cout << gate.in0 << " | " << gate.in1 << " -> " << gate.out << std::endl;
                }else if(gate.opi == XOR){
                    circuit.state[gate.out] = circuit.state[gate.in0] ^ circuit.state[gate.in1];
                    //std::cout << gate.in0 << " ^ " << gate.in1 << " -> " << gate.out << std::endl;
                }

                op_count++;

                if(gate.out[0] == 'z'){
                    int zi = (gate.out[1] - '0') * 10 + (gate.out[2] - '0');
                    z_set |= 1ull << zi;
                }
            }
        }
    }

    uint64_t binary = 0;
    for(int z=0; z<circuit.zcount; ++z){
        if(circuit.state[(z < 10 ? "z0" : "z") + std::to_string(z)] == 1){
            binary |= 1ull << z;
        }
    }

    return std::make_tuple(x_binary, y_binary, binary);
}

/* 
int yi = ((gate.i_out / (36)) % (36));
int xi = gate.i_out % 36;
int zi = yi * 10 + xi;
*/



uint64_t process2(circuit_t circuit)
{
    scoped_timer t(e_microseconds); 

    uint64_t z_binary = 0;
    uint64_t z_has_seen = 0;
    uint64_t z_all_seen = (1ull << circuit.zcount) - 1;

    int runs = 0;
    int max_runs = (circuit.zcount+1)*2;

    while(z_has_seen != z_all_seen && runs < max_runs)
    {
        runs++;

        for(auto& gate : circuit.gates){
            if((circuit.statei[gate.i_in0] != -1) && (circuit.statei[gate.i_in1] != -1) && z_has_seen != z_all_seen)
            {
                if(gate.opi == AND){
                    circuit.statei[gate.i_out] = circuit.statei[gate.i_in0] & circuit.statei[gate.i_in1];
                }else if(gate.opi == OR){
                    circuit.statei[gate.i_out] = circuit.statei[gate.i_in0] | circuit.statei[gate.i_in1];
                }else if(gate.opi == XOR){
                    circuit.statei[gate.i_out] = circuit.statei[gate.i_in0] ^ circuit.statei[gate.i_in1];
                }

                if(z_idx(gate.i_out) == Z){
                    int zi = y_idx(gate.i_out)*10 + x_idx(gate.i_out);
                    z_has_seen |= 1ull << zi;

                    if(circuit.statei[gate.i_out] == 1){
                        z_binary |= 1ull << zi;
                    }else{
                        z_binary &= ~(1ull << zi);
                    }
                }
            }
        }
    }

    std::cout << "gate count: " << std::count_if(circuit.statei.begin(), circuit.statei.end(), [](int i){ return i != -1; }) << std::endl;

    return z_binary;
}


auto process3(circuit_t circuit)
{
    //scoped_timer t(e_microseconds); 

    uint64_t y_binary = 0;
    for(int y=0; y<circuit.ycount; ++y){
        if(circuit.state[(y < 10 ? "y0" : "y") + std::to_string(y)] == 1){
            y_binary |= 1ull << y;
        }
    }

    uint64_t x_binary = 0;
    for(int x=0; x<circuit.xcount; ++x){
        if(circuit.state[(x < 10 ? "x0" : "x") + std::to_string(x)] == 1){
            x_binary |= 1ull << x;
        }
    }

    //std::cout << std::endl;

    //std::cout << "number outputs: " << circuit.opsi.size() << std::endl;

    //std::cout << "expected: " << x_binary << " + " << y_binary << " = " << x_binary + y_binary << std::endl;

    uint64_t z_binary = 0;
    uint64_t z_has_seen = 0;
    uint64_t z_all_seen = (1ull << circuit.zcount) - 1;

    int runs = 0;
    int max_runs = (circuit.zcount+1)*10;

    while(z_has_seen != z_all_seen && runs < max_runs)
    {
        //for(auto& gate : circuit.gates){
        for(int i=0; i<circuit.opsi.size(); ++i){
            //int j = i * 3;
            int j = i;
            //int in0 = 
            //if((circuit.statei[gate.i_in0] != -1) && (circuit.statei[gate.i_in1] != -1) && z_has_seen != z_all_seen)
            //if((circuit.statei[circuit.gatesi[j]] != -1) && (circuit.statei[circuit.gatesi[j+1]] != -1) && z_has_seen != z_all_seen)
            if((circuit.in0(j) != -1) && (circuit.in1(j) != -1) && z_has_seen != z_all_seen)
            {
                //if(gate.opi == AND){
                if(circuit.opsi[i] == AND){
                    //circuit.statei[gate.i_out] = circuit.statei[gate.i_in0] & circuit.statei[gate.i_in1];
                    //circuit.statei[circuit.gatesi[j+2]] = circuit.statei[circuit.gatesi[j+0]] & circuit.statei[circuit.gatesi[j+1]];
                    circuit.out(j) = circuit.in0(j) & circuit.in1(j);
                }else if(circuit.opsi[i] == OR){
                    //circuit.statei[gate.i_out] = circuit.statei[gate.i_in0] | circuit.statei[gate.i_in1];
                    //circuit.statei[circuit.gatesi[j+2]] = circuit.statei[circuit.gatesi[j+0]] | circuit.statei[circuit.gatesi[j+1]];
                    circuit.out(j) = circuit.in0(j) | circuit.in1(j);
                }else if(circuit.opsi[i] == XOR){
                    //circuit.statei[gate.i_out] = circuit.statei[gate.i_in0] ^ circuit.statei[gate.i_in1];
                    //circuit.statei[circuit.gatesi[j+2]] = circuit.statei[circuit.gatesi[j+0]] ^ circuit.statei[circuit.gatesi[j+1]];
                    circuit.out(j) = circuit.in0(j) ^ circuit.in1(j);
                }

                /*if(z_idx(circuit.gatesi[j+2]) == Z){
                    int zi = y_idx(circuit.gatesi[j+2])*10 + x_idx(circuit.gatesi[j+2]);
                    z_has_seen |= 1ull << zi;

                    if(circuit.statei[circuit.gatesi[j+2]] == 1){
                        z_binary |= 1ull << zi;
                    }else{
                        z_binary &= ~(1ull << zi);
                    }
                }*/

                if(z_idx(circuit.out_data[j]) == Z){
                    int zi = y_idx(circuit.out_data[j])*10 + x_idx(circuit.out_data[j]);
                    z_has_seen |= 1ull << zi;

                    if(circuit.statei[circuit.out_data[j]] == 1){
                        z_binary |= 1ull << zi;
                    }else{
                        z_binary &= ~(1ull << zi);
                    }
                }

                /*if(z_idx(circuit.out_data[j]) == Y){
                    int zi = y_idx(circuit.out_data[j])*10 + x_idx(circuit.out_data[j]);
                    z_has_seen |= 1ull << zi;

                    if(circuit.statei[circuit.out_data[j]] == 1){
                        z_binary |= 1ull << zi;
                    }else{
                        z_binary &= ~(1ull << zi);
                    }
                }*/
            }
        }

        runs++;
    }

    //std::cout << "actual:   " << x_binary << " + " << y_binary << " = " << z_binary << std::endl;

    //std::cout << "gate count: " << std::count_if(circuit.statei.begin(), circuit.statei.end(), [](int i){ return i != -1; }) << std::endl;

    return std::make_tuple(x_binary, y_binary, z_binary);
}


uint64_t part1(circuit_t circuit)
{
    auto [x_binary, y_binary, z_binary] = process3(circuit);
    return z_binary;
}

void swapPairs(std::vector<int>& values, const std::vector<int>& group1, const std::vector<int>& group2) {
    // Swap the values between the two groups
    for (int i = 0; i < 4; ++i) {
        std::swap(values[group1[i]], values[group2[i]]);
    }
}

int part2(const circuit_t& orig_circuit)
{
    //auto [x_binary, y_binary, z_binary] = process3(circuit);

    size_t n = orig_circuit.out_data.size();

    int i = 0;

    for(int a=0; a<n; ++a){
        for(int b=a+1; b<n; ++b){
            for(int c=b+1; c<n; ++c){
                for(int d=c+1; d<n; ++d){

                     for(int a0=d+1; a0<n; ++a0){
                        for(int b0=a0+1; b0<n; ++b0){
                            for(int c0=b0+1; c0<n; ++c0){
                                for(int d0=c0+1; d0<n; ++d0){

                                    auto circuit = orig_circuit;

                                    std::swap(circuit.out_data[a], circuit.out_data[b]);
                                    std::swap(circuit.out_data[c], circuit.out_data[d]);

                                    std::swap(circuit.out_data[a0], circuit.out_data[b0]);
                                    std::swap(circuit.out_data[c0], circuit.out_data[d0]);

                                    auto [x_binary, y_binary, z_binary] = process3(circuit);

                                    if(x_binary + y_binary == z_binary){
                                        std::cout << "success:   " << x_binary << " + " << y_binary << " = " << z_binary << std::endl;
                                        std::cout << a << "," << b << "," << c << "," << d << "," << std::endl;
                                        std::cout << a0 << "," << b0 << "," << c0 << "," << d0 << "," << std::endl;
                                        return 100;
                                    }

                                    i++;
                                    if((i % 1000) == 0){
                                        std::cout << i << std::endl;
                                    }

                                }
                            }
                        }
                    }
        
                }
            }
        }
    }
    
    /*// Generate all combinations of 4 elements from the 8 values
    std::vector<int> indices(n);
    std::iota(indices.begin(), indices.end(), 0);  // Fill indices with 0 to 7

    int i = 0;
    
    do {
        // Select the first group of 4 elements (first half)
        std::vector<int> group1 = {indices[0], indices[1], indices[2], indices[3]};
        // Select the second group of 4 elements (second half)
        std::vector<int> group2 = {indices[4], indices[5], indices[6], indices[7]};
        
        auto circuit = orig_circuit;

        // Perform the swap of the two groups
        swapPairs(circuit.out_data, group1, group2);

        auto [x_binary, y_binary, z_binary] = process3(circuit);

        if(x_binary + y_binary == z_binary){
            std::cout << "success:   " << x_binary << " + " << y_binary << " = " << z_binary << std::endl;
        }
        
        // Print the result after swap
        / *std::cout << "After swap: ";
        for (int val : values) {
            std::cout << val << " ";
        }
        std::cout << std::endl;* /

        // Optional: Revert the swap to test the next case
        //swapPairs(circuit.out_data, group1, group2);

        i++;
        if((i % 1000) == 0){
            std::cout << i << std::endl;
        }
    } while (std::next_permutation(indices.begin(), indices.end()));*/


    return 0;
}


std::string get_op(int i){
    if(i==AND){
        return "AND";
    }else if(i==OR){
        return "OR";
    }else /*if(i==XOR)*/{
        return "XOR";
    } 
};


 /* got to my solution by first graphing all the nodes. It showed that the graph was made of 1 bit adders and there were no super wild edge going very far. So you could just find the adders that were wrong.

So I wrote some rules to check the graph, first checked that all zXX were output by a XOR. It allowed me to find one error nearby. The second rule I wrote to find the wrong ones was that OR nodes must have 2 AND parents. Found 3 more errors that way and identified the swap by looking at the graph.

That's 4 swaps, done! The problem might have been more interesting if we didn't know how many swaps there were. */

int part3(circuit_t orig_circuit){

    std::set<std::string> outs;
    std::set<int> iouts;

    std::cout << std::endl;
    for(int i=0; i<orig_circuit.opsi.size(); ++i){
        if(z_idx(orig_circuit.out_data[i]) == Z && orig_circuit.opsi[i] != XOR){
            std::cout << "NO XOR FOR Z! " << inv_base36Hash(orig_circuit.in0_data[i]) << " " << get_op(orig_circuit.opsi[i]) << " " << inv_base36Hash(orig_circuit.in1_data[i]) << " -> " << inv_base36Hash(orig_circuit.out_data[i]) << std::endl;
            outs.insert(orig_circuit.gates[i].out);
            iouts.insert(i);
        }

        if(orig_circuit.opsi[i] == OR){

            auto in0 = orig_circuit.in0_data[i];
            auto in1 = orig_circuit.in1_data[i];

            int and_count = 0;
            for(int j=0; j<orig_circuit.opsi.size(); ++j){
                if(orig_circuit.out_data[j] == in0 && orig_circuit.opsi[j] == AND){
                    and_count++;
                }

                if(orig_circuit.out_data[j] == in1 && orig_circuit.opsi[j] == AND){
                    and_count++;
                }
            }

            if(and_count < 2){
                outs.insert(orig_circuit.gates[i].out);
                iouts.insert(i);
                std::cout << "****************** OR! " << inv_base36Hash(orig_circuit.in0_data[i]) << " " << get_op(orig_circuit.opsi[i]) << " " << inv_base36Hash(orig_circuit.in1_data[i]) << " -> " << inv_base36Hash(orig_circuit.out_data[i]) << std::endl;
            }
        }

        std::cout << inv_base36Hash(orig_circuit.in0_data[i]) << " " << get_op(orig_circuit.opsi[i]) << " " << inv_base36Hash(orig_circuit.in1_data[i]) << " -> " << inv_base36Hash(orig_circuit.out_data[i]) << std::endl;
    }
    std::cout << std::endl;

    for(auto& s : iouts){
        std::cout << s << ",";
    }
    std::cout << std::endl;

    int j = 0;
    //for(int i=0; i<orig_circuit.opsi.size(); ++i){
    for(int i=(int)orig_circuit.opsi.size()-1; i>=0; --i){
        std::set<int> iouts2 = iouts;
        iouts2.insert(i);
        if(iouts2.size() != 8){
            continue;
        }
        std::vector<int> vouts(iouts2.begin(), iouts2.end());

        do{
            auto circuit = orig_circuit;

            std::swap(circuit.out_data[vouts[0]], circuit.out_data[vouts[1]]);
            std::swap(circuit.out_data[vouts[2]], circuit.out_data[vouts[3]]);

            std::swap(circuit.out_data[vouts[4]], circuit.out_data[vouts[5]]);
            std::swap(circuit.out_data[vouts[6]], circuit.out_data[vouts[7]]);

            auto [x_binary, y_binary, z_binary] = process3(circuit);

            if(x_binary + y_binary == z_binary){
                std::cout << "success:   " << x_binary << " + " << y_binary << " = " << z_binary << std::endl;
                //std::cout << a << "," << b << "," << c << "," << d << "," << std::endl;
                //std::cout << a0 << "," << b0 << "," << c0 << "," << d0 << "," << std::endl;
                ///return 100;
                std::cout << "vouts:" << std::endl;
                for(auto& s : vouts){
                    std::cout << s << ",";
                }
                return 999999999;
            }

            j++;
            if((j % 1000) == 0){
                std::cout << j << std::endl;
            }

        } while (std::next_permutation(vouts.begin(), vouts.end()));

        std::cout << "******************************************* " << i << " / " << orig_circuit.opsi.size() << std::endl;
    }

    return 0;
}


int part4(circuit_t orig_circuit)
{
    std::set<int> iouts;
    std::set<wire_t> wires;

    std::cout << orig_circuit.zcount << std::endl;

    std::cout << std::endl;
    /*for(int i=0; i<orig_circuit.opsi.size(); ++i){
        if(z_idx(orig_circuit.out_data[i]) == Z && orig_circuit.opsi[i] != XOR && inv_base36Hash(orig_circuit.out_data[i]) != "z45"){
            std::cout << "NO XOR FOR Z! " << inv_base36Hash(orig_circuit.in0_data[i]) << " " << get_op(orig_circuit.opsi[i]) << " " << inv_base36Hash(orig_circuit.in1_data[i]) << " -> " << inv_base36Hash(orig_circuit.out_data[i]) << std::endl;
            iouts.insert(i);
        }

        if(z_idx(orig_circuit.out_data[i]) != Z && (z_idx(orig_circuit.in0_data[i]) != Y || z_idx(orig_circuit.in1_data[i]) != X) && (z_idx(orig_circuit.in0_data[i]) != X || z_idx(orig_circuit.in1_data[i]) != Y)){
            if(orig_circuit.opsi[i] == XOR){
                std::cout << "only XOR for non x,y and z gates! " << inv_base36Hash(orig_circuit.in0_data[i]) << " " << get_op(orig_circuit.opsi[i]) << " " << inv_base36Hash(orig_circuit.in1_data[i]) << " -> " << inv_base36Hash(orig_circuit.out_data[i]) << std::endl;
                iouts.insert(i);
            }
        }



    }*/

    for(auto& gate : orig_circuit.gates){
        if(gate.out[0]=='z' && gate.op != "XOR" && gate.out != "z45"){
            std::cout << "NO XOR FOR Z! " << gate.in0 << " " << gate.op << " " << gate.in1 << " -> " << gate.out << std::endl;
            wires.insert(gate.out);
        }

        if(gate.out[0]!='z' && (gate.in0[0] != 'x' || gate.in1[0] != 'y') && (gate.in0[0] != 'y' || gate.in1[0] != 'x') && gate.op == "XOR"){
            std::cout << "only XOR for non x,y and z gates! " << gate.in0 << " " << gate.op << " " << gate.in1 << " -> " << gate.out << std::endl;
            wires.insert(gate.out);
        }

        auto is_xy_in0 = [](const gate_t& gate){ return gate.in0[0] == 'x' || gate.in0[0] == 'y'; };
        auto is_xy_in1 = [](const gate_t& gate){ return gate.in1[0] == 'x' || gate.in1[0] == 'y'; };
        auto is_first_in0 = [](const gate_t& gate){ return gate.in0.substr(1) == "00"; };
        auto is_first_in1 = [](const gate_t& gate){ return gate.in1.substr(1) == "00"; };

        if (gate.op == "XOR" && is_xy_in0(gate) && is_xy_in1(gate)) 
        {
            //if (!(gate.in0.endsWith("00") && c.getOperand2().endsWith("00"))) {
            if(!is_first_in0(gate) && !is_first_in1(gate)) {
                //final String output = c.getOutputWire();
                //boolean anotherFound = false;
                bool anotherFound = false;
                for (auto& gate0 : orig_circuit.gates) {
                    //if (!gate0.out != .equals(c)) {
                        //if ((c2.getOperand1().equals(output) || c2.getOperand2().equals(output)) && c2.getOperator().equals("XOR")) {
                        if((gate0.in0 == gate.out || gate0.in1 == gate.out) && gate0.op == "XOR") {
                            anotherFound = true;
                            break;
                        }
                    //}
                }
                if (!anotherFound) {
                    std::cout << "non xy inputs with xor! " << gate.in0 << " " << gate.op << " " << gate.in1 << " -> " << gate.out << std::endl;
                    wires.insert(gate.out);
                }
            }
        }

        if (gate.op == "AND" && is_xy_in0(gate) && is_xy_in1(gate)) 
        {
            //if (!(gate.in0.endsWith("00") && c.getOperand2().endsWith("00"))) {
            if(!is_first_in0(gate) && !is_first_in1(gate)) {
                //final String output = c.getOutputWire();
                //boolean anotherFound = false;
                bool anotherFound = false;
                for (auto& gate0 : orig_circuit.gates) {
                    //if (!gate0.out != .equals(c)) {
                        //if ((c2.getOperand1().equals(output) || c2.getOperand2().equals(output)) && c2.getOperator().equals("XOR")) {
                        if((gate0.in0 == gate.out || gate0.in1 == gate.out) && gate0.op == "OR") {
                            anotherFound = true;
                            break;
                        }
                    //}
                }
                if (!anotherFound) {
                    std::cout << "non xy inputs with xor! " << gate.in0 << " " << gate.op << " " << gate.in1 << " -> " << gate.out << std::endl;
                    wires.insert(gate.out);
                }
            }
        }

    }

    std::vector<wire_t> vwires(wires.begin(), wires.end());

    int j = 0;
    do{
        auto circuit = orig_circuit;

        auto swap = [&](const wire_t& a, const wire_t& b){
            auto it_a = std::find_if(circuit.gates.begin(), circuit.gates.end(), [&](auto& gate){ return gate.out == a; }); 
            auto it_b = std::find_if(circuit.gates.begin(), circuit.gates.end(), [&](auto& gate){ return gate.out == b; }); 
            std::swap(it_a->out, it_b->out);
        };

        swap(vwires[0], vwires[1]);
        swap(vwires[2], vwires[3]);
        swap(vwires[4], vwires[5]);
        swap(vwires[6], vwires[7]);

        auto [x_binary, y_binary, z_binary] = process(circuit);

        if(x_binary + y_binary == z_binary){
            std::cout << "success:   " << x_binary << " + " << y_binary << " = " << z_binary << std::endl;
            return 999999999;
        }

        j++;
        if((j % 100) == 0){
            std::cout << j << std::endl;
        }
    } while (std::next_permutation(vwires.begin(), vwires.end()));

    std::cout << "wires:" << std::endl;
    for(auto& s : wires){
        std::cout << s << ",";
    }
    std::cout << std::endl;

    return 0;
}

void main()
{
    auto test_values0 = load_input("../src/day24/test_input0.txt");
    auto test_values1 = load_input("../src/day24/test_input1.txt");
    auto actual_values = load_input("../src/day24/input.txt");

    std::cout << "part1: " << part1(test_values0) << std::endl;
    std::cout << "part1: " << part1(test_values1) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    //std::cout << "part2: " << part2(actual_values) << std::endl;
    std::cout << "part2: " << part4(actual_values) << std::endl;

    //std::cout << "part2: " << part2(test_values) << std::endl;
    //std::cout << "part2: " << part2(actual_values) << std::endl;
}