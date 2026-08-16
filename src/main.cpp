#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>
#include "ags/common.hpp"
#include "ags/rabin_karp.hpp"

using namespace std;

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

    if (fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                processFile(entry.path().string(), pattern, caseInsensitive);
            }
        }
    } else {
        processFile(path, pattern, caseInsensitive);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "------------------------------------------" << endl;
    cout << "Search Results for: " << pattern << (caseInsensitive ? " (Case-Insensitive)" : "") << endl;
    cout << "Total Matches: " << allMatches.size() << endl;
    cout << "Time Elapsed: " << elapsed.count() << " seconds" << endl;
    cout << "------------------------------------------" << endl;

    for (const auto& m : allMatches) {
        cout << "Found in " << m.filename << " at byte " << m.offset << endl;
    }

    return 0;
}
