#pragma once
#include <string>
#include <cstdlib>

class ThreadPool;
class FileGenerator
{
private:
    ThreadPool &threadPool;
    std::string file_path;
    void createFile(std::string id);
public:
    explicit FileGenerator(ThreadPool &pool);
    ~FileGenerator() = default;

    void runLoop();
};