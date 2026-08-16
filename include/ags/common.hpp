#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <mutex>
#include <algorithm>

namespace fs = std::filesystem;


struct Match {
    std::string filename;
    size_t lineNum;
    size_t offset;
    std::string preview; // The actual line text
};


struct SearchStats {
    int totalMatches = 0;
    int filesProcessed = 0;
    double timeTaken = 0.0;
};
