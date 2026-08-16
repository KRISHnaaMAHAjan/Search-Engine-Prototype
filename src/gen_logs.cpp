#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>

int main() {
    std::ofstream out("sample_logs/large_log.log");
    if (!out) {
        std::cerr << "Failed to create sample_logs directory or file.\n";
        return 1;
    }

    std::vector<std::string> messages = {
        "INFO: User logged in",
        "DEBUG: Cache hit for key 0x123",
        "ERROR: Database connection failed",
        "WARN: Low disk space",
        "TRACE: Entering function process_request",
        "INFO: Request processed in 20ms"
    };

    std::mt19937 rng(42);
    std::uniform_int_distribution<size_t> dist(0, messages.size() - 1);

    std::cout << "Generating 100,000 lines of log data...\n";
    for (int i = 0; i < 100000; ++i) {
        out << "2026-05-12 10:00:" << (i % 60) << " " << messages[dist(rng)] << "\n";
        if (i == 50000) {
            out << "2026-05-12 10:00:30 CRITICAL: System overheating! (TEST_PATTERN)\n";
        }
    }

    std::cout << "Done. File created: sample_logs/large_log.log\n";
    return 0;
}
