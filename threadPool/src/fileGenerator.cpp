#include "fileGenerator.h"
#include "threadPool.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

FileGenerator::FileGenerator(ThreadPool &pool) : threadPool(pool) {
    const char* env_p = std::getenv("GENERATE_FILE_PATH");
    if (env_p) {
        file_path = env_p;
        try {
            if (!fs::exists(file_path)) {
                fs::create_directories(file_path);
                std::cout << "Directory created at: " << file_path << "\n";
            } else {
                std::cout << "File path initialized: " << file_path << "\n";
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << "\n";
            // Handle error (e.g., throw, fallback, or set safe state)
            throw; 
        }
    }else{
        // Fallback strategy or Error
        std::string errorMsg = "GENERATE_FILE_PATH not set.";
        std::cerr << errorMsg << "\n";
        throw std::runtime_error(errorMsg);
    }
}

void FileGenerator::createFile(std::string id){
    auto job = [id, path = this->file_path](){
        std::string filename = path + "/file" + id + ".txt";
        std::ofstream outfile(filename);
        if(outfile.is_open()){
            outfile << "Content for " << id << std::endl;
            outfile.close();
            // Thread-safe logging requires a mutex usually, using cout for simplicity here
            // In a real system, use a thread-safe logger.
            std::cout << "[Thread " << std::this_thread::get_id() 
                      << "] Created " << filename << "\n";
        }
    };

    threadPool.enqueueTask(job);
}

void FileGenerator::runLoop(){
    int counter = 0;
    while(true){
        try
        {
            createFile(std::to_string(counter++));
            std::this_thread::sleep_for(std::chrono::seconds(2)); 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            break;
        }
        
    }
}