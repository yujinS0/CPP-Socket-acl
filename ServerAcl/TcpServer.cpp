#include <thread>
#include "pch.h"
#include "tcpServer.h"

void handle_client(acl::socket_stream* conn) {
    char buf[256];
    while (true) {
        int ret = conn->read(buf, sizeof(buf), false);  // Ŭ���̾�Ʈ�κ��� ������ �б�
        if (ret <= 0) break;  // ������ ����ǰų� ���� �߻� �� ���� Ż��

        conn->write(buf, ret);  // ���� �����͸� Ŭ���̾�Ʈ�� ����
    }
    delete conn;  // ���� ���� �� ���� ����
}

void run_tcp_server() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;

    if (!server.open(addr)) {  // ���� �ּҿ� ���ε��ϰ� ����
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return;
    }

    std::cout << "Server is running on " << addr << std::endl;

    while (true) {
        acl::socket_stream* conn = server.accept(); // Ŭ���̾�Ʈ ���� ���
        if (conn == nullptr) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        std::thread client_thread(handle_client, conn);  // �� �����忡�� Ŭ���̾�Ʈ ó��
        client_thread.detach();  // �����带 �и��Ͽ� ���������� ����ǵ��� ��
    }
}

