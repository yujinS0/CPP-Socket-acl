#include "pch.h"
#include "acl_cpp/lib_acl.hpp"

int main() {
    const char* addr = "127.0.0.1:8088";
    int conn_timeout = 5, rw_timeout = 10;
    acl::acl_cpp_init();  // ACL 초기화
    acl::socket_stream conn;

    if (!conn.open(addr, conn_timeout, rw_timeout)) {  // 서버에 연결
        std::cerr << "Failed to connect to server at " << addr << std::endl;
        return -1;
    }

    std::string message;
    std::cout << "Enter a message: ";
    std::getline(std::cin, message);

    if (conn.write(message.c_str(), message.size()) == -1) {  // 서버로 데이터 전송
        std::cerr << "Failed to send data to server" << std::endl;
        return -1;
    }

    char buf[256];
    int ret = conn.read(buf, sizeof(buf) - 1, false);
    if (ret > 0) {  // 서버로부터 데이터 수신
        buf[ret] = '\0';
        std::cout << "Echo from server: " << buf << std::endl;
    }
    else {
        std::cerr << "Failed to read data from server" << std::endl;
    }

    return 0;
}
