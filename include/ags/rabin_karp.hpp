#pragma once

#include <string>
#include <vector>
#include <cmath>


class RabinKarp {
private:
    std::string pattern;
    long long patternHash;
    long long prime = 1000000007; 
    int base = 256;               
    long long h = 1;              
    bool caseInsensitive;

public:
    RabinKarp(std::string p, bool ci = false) : pattern(p), caseInsensitive(ci) {
        int m = pattern.length();
        patternHash = 0;

        for (int i = 0; i < m - 1; i++) {
            h = (h * base) % prime;
        }

        for (int i = 0; i < m; i++) {
            char c = caseInsensitive ? std::tolower(pattern[i]) : pattern[i];
            patternHash = (base * patternHash + (unsigned char)c) % prime;
        }
    }

    std::vector<size_t> search(const std::string& text) {
        std::vector<size_t> results;
        int n = text.length();
        int m = pattern.length();
        long long currentHash = 0;

        if (n < m) return results;

        for (int i = 0; i < m; i++) {
            char c = caseInsensitive ? std::tolower(text[i]) : text[i];
            currentHash = (base * currentHash + (unsigned char)c) % prime;
        }

        for (int i = 0; i <= n - m; i++) {
            if (patternHash == currentHash) {
                bool match = true;
                for (int j = 0; j < m; j++) {
                    char c1 = caseInsensitive ? std::tolower(pattern[j]) : pattern[j];
                    char c2 = caseInsensitive ? std::tolower(text[i + j]) : text[i + j];
                    if (c1 != c2) {
                        match = false;
                        break;
                    }
                }
                if (match) results.push_back(i);
            }

            if (i < n - m) {
                char c_old = caseInsensitive ? std::tolower(text[i]) : text[i];
                char c_new = caseInsensitive ? std::tolower(text[i + m]) : text[i + m];

                currentHash = (base * (currentHash - (unsigned char)c_old * h) + (unsigned char)c_new) % prime;
                
                if (currentHash < 0) {
                    currentHash += prime;
                }
            }
        }
        return results;
    }
};
