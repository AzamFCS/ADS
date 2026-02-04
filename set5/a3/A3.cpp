#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <set>
#include <cmath>
#include <iomanip>

class RandomStreamGen {
public:
    std::string generate_string(int length) {
        static const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-";
        std::string result;
        for (int i = 0; i < length; ++i) {
            result += chars[rand() % (sizeof(chars) - 1)];
        }
        return result;
    }

    std::vector<std::string> generate_stream(int num_strings, int max_length) {
        std::vector<std::string> stream;
        for (int i = 0; i < num_strings; ++i) {
            stream.push_back(generate_string(rand() % max_length + 1));
        }
        return stream;
    }
};

class HashFuncGen {
public:
    // Используем uint32_t для соответствия условию 2^32
    static uint32_t fnv1a_hash(const std::string& str) {
        uint32_t hash = 2166136261u;
        for (char c : str) {
            hash ^= static_cast<uint8_t>(c);
            hash *= 16777619;
        }
        return hash;
    }
};

class HyperLogLog {
private:
    std::vector<uint8_t> registers;
    size_t b;
    size_t m;
    double alphaMM;

public:
    HyperLogLog(size_t b) : b(b) {
        m = 1 << b;
        registers.assign(m, 0);
        
        // Определение константы alphaMM
        if (m == 16) alphaMM = 0.673;
        else if (m == 32) alphaMM = 0.697;
        else if (m == 64) alphaMM = 0.709;
        else alphaMM = 0.7213 / (1 + 1.079 / m);
    }

    void add(const std::string& element) {
        uint32_t hash = HashFuncGen::fnv1a_hash(element);
        uint32_t idx = hash >> (32 - b); 
        uint32_t w = hash << b;          
        uint8_t rank = 1;
        if (w != 0) {
            rank = __builtin_clz(w) + 1;
        } else {
            rank = (32 - b) + 1;
        }

        if (rank > registers[idx]) {
            registers[idx] = rank;
        }
    }

    double estimate() {
        double sum = 0.0;
        for (uint8_t val : registers) {
            sum += std::pow(2.0, -val);
        }

        double E = alphaMM * m * m / sum;

        if (E <= 2.5 * m) {
            int V = 0;
            for (uint8_t val : registers) {
                if (val == 0) V++;
            }
            if (V > 0) {
                E = m * std::log(static_cast<double>(m) / V);
            }
        }
        else if (E > (1.0 / 30.0) * 4294967296.0) {
            E = -4294967296.0 * std::log(1.0 - E / 4294967296.0);
        }

        return E;
    }
};

int main() {
    srand(4);
    RandomStreamGen streamGen;
    
    size_t num_strings = 10000;
    size_t b = 10; 
    
    auto stream = streamGen.generate_stream(num_strings, 30);
    HyperLogLog hll(b);
    std::set<std::string> exactSet;
    std::string s = "results" + std::to_string(4) + ".csv";
    std::ofstream outFile(s);
    outFile << "Step,Exact_Count,Estimated_Count\n";

    std::cout << "Processing stream..." << std::endl;

    for (size_t i = 0; i < stream.size(); ++i) {
        hll.add(stream[i]);
        exactSet.insert(stream[i]); 
        if ((i + 1) % 100 == 0) {
            outFile << (i + 1) << "," << exactSet.size() << "," 
                    << std::fixed << std::setprecision(2) << hll.estimate() << "\n";
        }
    }

    outFile.close();
    std::cout << "Done! Results saved to results.csv" << std::endl;
    return 0;
}