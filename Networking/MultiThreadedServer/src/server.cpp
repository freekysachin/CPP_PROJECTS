#include "server.h"

TCPServer::~TCPServer(){
    stop();
}

void TCPServer::stop(){
    if (!m_running) return;
    m_running = false;

    if (m_server_fd != -1) {
        ::shutdown(m_server_fd, SHUT_RDWR);
    }

    std::cout << "Waiting for active connections to finish..." << std::endl;
    std::lock_guard<std::mutex> lock(m_worker_mtx);
    for(int i =0; i< m_worker_threads.size(); i++){
        std::cout<<"Shutting down "<<m_worker_threads[i].get_id()<<std::endl;
        if(m_worker_threads[i].joinable()) m_worker_threads[i].join();
    }
    m_worker_threads.clear();
    std::cout<<"All Connection ShutDown\n";

    if (m_server_fd != -1) {
        ::close(m_server_fd);
        m_server_fd = -1;
    }
}

void TCPServer::bind(uint16_t port){
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_server_fd < 0) throw std::runtime_error("Socket Creation Failed");

    int opt = 1;
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port = port);
    if(::bind(m_server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        throw std::runtime_error("Bind failed on port " + std::to_string(m_port));
    }
}

void TCPServer::listen(){
    if(::listen(m_server_fd, SOMAXCONN) < 0){
        throw std::runtime_error("Listen Failed");
    }
    m_running = true;
    std::cout << "Server started on port " << m_port << std::endl;
    TCPLoop();
}

void TCPServer::TCPLoop(){
    while (m_running) {
        int client_fd = accept(m_server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            if (!m_running) break;
            if (m_running) perror("Accept failed");
            continue;
        }
        std::lock_guard<std::mutex> lock(m_worker_mtx);
        m_worker_threads.emplace_back([this, client_fd]() {
            this->handle_client(client_fd);
        });
    }
}

void TCPServer::handle_client(int client_fd){
    char buffer[1024] = {0};
    read(client_fd, buffer, sizeof(buffer));

    std::string body = "<h1>Hello From Server</h1>";
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + 
        body;
    
    send(client_fd, response.c_str(), response.size(), 0);
    close(client_fd);
}