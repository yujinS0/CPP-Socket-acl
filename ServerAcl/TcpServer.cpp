#include <thread>
#include "pch.h"

void run(void) {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server; // server_socket Ŭ���� �ν��Ͻ� ����
    if (!server.open(addr)) {  // addr �ּҷ� ���� ������ ����, Bind�ؼ� Ŭ���̾�Ʈ�� ���� ��û�� ��� ����(listen)�� �����
        return; // ���� �� �ٷ� ����
    }

    while (true) {
        acl::socket_stream* conn = server.accept(); // Wait for connection. Ŭ���̾�Ʈ�� ���� ��û ��� 
                                                    // -> ���� Ŭ���̾�Ʈ�� ���� �õ��ϸ�, ���ο� socket_stream ��ü ��ȯ
        if (conn == NULL) {
            break;
        }
        std::thread thread([=] {  // Start one thread to handle the connection (������ ����)
            char buf[256];  // �ִ� 256 ����Ʈ ������
            int ret = conn->read(buf, sizeof(buf), false);  // Read data 
            if (ret > 0) {
                conn->write(buf, ret);  // ���� ������ �ٽ� ����
            }
            delete conn; // ���� ������, socket_stream ��ü ����
            });
        thread.detach();  // ������ ������ detach (������ ������� ���� ������� ���������� ����)
                          // => ���� �����尡 ����Ǿ �����尡 ���������� ��� ����.
    }
}

void handle_client(acl::socket_stream* conn) {
    char buf[256];
    while (true) {
        int ret = conn->read(buf, sizeof(buf), false);  // Ŭ���̾�Ʈ�κ��� ������ �б�
        if (ret <= 0) break;  // ������ ����ǰų� ���� �߻� �� ���� Ż��

        conn->write(buf, ret);  // ���� �����͸� Ŭ���̾�Ʈ�� ����
    }
    delete conn;  // ���� ���� �� ���� ����
}

int main() {
    const char* addr = "127.0.0.1:8088";
    acl::acl_cpp_init();  // ACL �ʱ�ȭ
    acl::server_socket server;

    if (!server.open(addr)) {  // ���� �ּҿ� ���ε��ϰ� ����
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return -1;
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

    return 0;
}