#pragma once
#include <cstdint>
#include <atomic>
#include <vector>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <mutex>

class TCPServer
{
private:
    int m_server_fd{-1};
    uint16_t m_port;
    std::atomic<bool> m_running{false};

    std::vector<std::thread> m_worker_threads;
    std::mutex m_worker_mtx;
    void handle_client(int client_fd);
    void TCPLoop();

public:
    TCPServer() = default;
    ~TCPServer();

    TCPServer( const TCPServer&) = delete;
    TCPServer& operator = (const TCPServer&) = delete;

    void bind(uint16_t port);
    void listen();
    void stop();

};
