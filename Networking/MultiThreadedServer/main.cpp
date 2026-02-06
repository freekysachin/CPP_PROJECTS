#include "server.h"
#include <csignal>

TCPServer* global_server_ptr = nullptr;

void handle_signal(int signal){
    if (global_server_ptr) {
        std::cout << "\n[Signal " << signal << "] Shutdown initiated..." << std::endl;
        global_server_ptr->stop();
    }
}

int main(){
    try {
        TCPServer server;
        global_server_ptr = &server;

        // Register SIGINT (Ctrl+C) and SIGTERM (System kill)
        std::signal(SIGINT, handle_signal);
        std::signal(SIGTERM, handle_signal);

        server.bind(3000);
        server.listen();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}