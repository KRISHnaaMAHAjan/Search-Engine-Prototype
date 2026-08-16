#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <cctype>
#include "ags/common.hpp"
#include "ags/rabin_karp.hpp"
#include "ags/bloom_filter.hpp"

using namespace std;

std::vector<std::string> tokenize(const std::string& text, bool case_insensitive) {
    std::vector<std::string> tokens;
    std::string current_token;
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            current_token += case_insensitive ? std::tolower(static_cast<unsigned char>(c)) : c;
        } else if (!current_token.empty()) {
            tokens.push_back(current_token);
            current_token.clear();
        }
    }
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    return tokens;
}

ags::BloomFilter buildBloomFilterForFile(const std::string& filepath, bool case_insensitive) {
    ags::BloomFilter bf(10000, 0.01); 
    std::ifstream file(filepath);
    if (!file.is_open()) return bf;
    std::string word;
    while (file >> word) {
        auto tokens = tokenize(word, case_insensitive);
        for (const auto& token : tokens) {
            bf.add(token);
        }
    }
    return bf;
}

// Mutex for protecting shared results
mutex resultMutex;
vector<Match> allMatches;

void searchInChunk(string filename, string pattern, long offset, int size, bool caseInsensitive) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return;

    file.seekg(offset);
    string buffer(size, '\0');
    file.read(&buffer[0], size);
    file.close();

    RabinKarp rk(pattern, caseInsensitive);
    vector<size_t> found = rk.search(buffer);

    if (!found.empty()) {
        lock_guard<mutex> lock(resultMutex);
        for (size_t pos : found) {
            Match m;
            m.filename = filename;
            m.offset = offset + pos;
            allMatches.push_back(m);
        }
    }
}

//multi-threading
void processFile(string filename, string pattern, bool caseInsensitive) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        cout << "Could not open file: " << filename << endl;
        return;
    }

    long fileSize = file.tellg();
    file.close();

    int numThreads = thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;

    long chunkSize = fileSize / numThreads;
    vector<thread> threads;

    for (int i = 0; i < numThreads; i++) {
        long start = i * chunkSize;
        int size = (i == numThreads - 1) ? (fileSize - start) : chunkSize;
        
        if (i < numThreads - 1) {
            size += pattern.length(); 
        }

        threads.push_back(thread(searchInChunk, filename, pattern, start, size, caseInsensitive));
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: searchtool [-i] <path> <pattern>" << endl;
        return 1;
    }

    bool caseInsensitive = false;
    string path;
    string pattern;

    if (argc == 4 && string(argv[1]) == "-i") {
        caseInsensitive = true;
        path = argv[2];
        pattern = argv[3];
    } else {
        path = argv[1];
        pattern = argv[2];
    }

    auto start = chrono::high_resolution_clock::now();

    vector<string> targetFiles;
    if (fs::exists(path)) {
        if (fs::is_directory(path)) {
            for (const auto& entry : fs::directory_iterator(path)) {
                if (entry.is_regular_file()) {
                    targetFiles.push_back(entry.path().string());
                }
            }
        } else {
            targetFiles.push_back(path);
        }
    } else {
        cout << "Path does not exist: " << path << endl;
        return 1;
    }

    unordered_map<string, ags::BloomFilter> bloomFilters;
    for (const auto& filepath : targetFiles) {
        bloomFilters.emplace(filepath, buildBloomFilterForFile(filepath, caseInsensitive));
    }

    vector<string> queryTokens = tokenize(pattern, caseInsensitive);

    int filesProcessed = 0;
    int filesSkipped = 0;
    int candidateFilesSearched = 0;

    for (const auto& filepath : targetFiles) {
        filesProcessed++;
        bool shouldSearch = true;
        
        if (!queryTokens.empty()) {
            const auto& bf = bloomFilters.at(filepath);
            for (const auto& token : queryTokens) {
                if (!bf.possiblyContains(token)) {
                    shouldSearch = false;
                    break;
                }
            }
        }

        if (shouldSearch) {
            candidateFilesSearched++;
            processFile(filepath, pattern, caseInsensitive);
        } else {
            filesSkipped++;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "------------------------------------------" << endl;
    cout << "Search Results for: " << pattern << (caseInsensitive ? " (Case-Insensitive)" : "") << endl;
    cout << "Files processed: " << filesProcessed << endl;
    cout << "Files skipped by Bloom Filter: " << filesSkipped << endl;
    cout << "Candidate files searched: " << candidateFilesSearched << endl;
    cout << "Total exact matches: " << allMatches.size() << endl;
    cout << "Time Elapsed: " << elapsed.count() << " seconds" << endl;
    cout << "------------------------------------------" << endl;

    for (const auto& m : allMatches) {
        cout << "Found in " << m.filename << " at byte " << m.offset << endl;
    }

    return 0;
}
