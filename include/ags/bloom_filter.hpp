#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <functional>
#include <algorithm>

namespace ags {

class BloomFilter {
private:
    std::vector<bool> bits;
    size_t num_bits;
    size_t num_hashes;

    uint32_t hash2(const std::string& str) const {
        uint32_t hash = 2166136261u;
        for (char c : str) {
            hash ^= static_cast<uint32_t>(c);
            hash *= 16777619u;
        }
        return hash;
    }

public:
    BloomFilter(size_t expected_elements = 10000, double false_positive_rate = 0.01) {
        double p = false_positive_rate;
        double n = static_cast<double>(expected_elements);
        
        double m = -(n * std::log(p)) / std::pow(std::log(2.0), 2.0);
        double k = (m / n) * std::log(2.0);

        num_bits = std::max(static_cast<size_t>(m), static_cast<size_t>(1));
        num_hashes = std::max(static_cast<size_t>(k), static_cast<size_t>(1));
        
        bits.resize(num_bits, false);
    }

    void add(const std::string& word) {
        std::hash<std::string> hasher1;
        uint32_t h1 = static_cast<uint32_t>(hasher1(word));
        uint32_t h2 = hash2(word);

        for (size_t i = 0; i < num_hashes; ++i) {
            size_t bit_index = (h1 + i * h2) % num_bits;
            bits[bit_index] = true;
        }
    }

    bool possiblyContains(const std::string& word) const {
        std::hash<std::string> hasher1;
        uint32_t h1 = static_cast<uint32_t>(hasher1(word));
        uint32_t h2 = hash2(word);

        for (size_t i = 0; i < num_hashes; ++i) {
            size_t bit_index = (h1 + i * h2) % num_bits;
            if (!bits[bit_index]) {
                return false; 
            }
        }
        return true; 
    }
};
}
