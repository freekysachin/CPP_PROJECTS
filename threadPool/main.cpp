#include "iostream"
#include <memory>
#include <sstream>
#include <algorithm>
#include <fstream>

#include "threadPool.h"
#include "fileGenerator.h"

void loadEnvFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        // Failing to find .env is not always fatal; 
        // the var might already be set in the OS.
        return; 
    }

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        // Parse KEY=VALUE
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            // Trim whitespace (crucial for robustness)
            // (Simple lambda to trim left and right)
            auto trim = [](std::string& s) {
                s.erase(0, s.find_first_not_of(" \t\n\r"));
                s.erase(s.find_last_not_of(" \t\n\r") + 1);
            };
            trim(key);
            trim(value);

            // Set environment variable only if not already set
            // overwrite = 0 (do not overwrite existing system envs)
            setenv(key.c_str(), value.c_str(), 0); 
        }
    }
}

int main(){
    loadEnvFile(".env");
    try {
        auto pool = std::make_unique<ThreadPool>(4);
        pool->init();

        auto fg = std::make_unique<FileGenerator>(*pool);
        fg->runLoop();
    }
    catch (const std::exception& e) {
        std::cerr << "Application Fatal Error: " << e.what() << "\n";
        return 1;
    }
}