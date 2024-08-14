#include "pch.h"
#include "tcpCoroutine.h"
#include "redisHandler.h"


void run_tcp_coroutine_server_with_redis() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;
    if (!server.open(addr)) {
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return;
    }

    std::cout << "Coroutine TCP Server is running on " << addr << std::endl;

    // Redis ���� ����
    const char* redis_addr = "127.0.0.1:6379";
    int conn_timeout = 10;  // Redis ���� Ÿ�Ӿƿ� (��)
    int rw_timeout = 10;    // Redis �б�/���� Ÿ�Ӿƿ� (��)

    // Redis Ŭ���̾�Ʈ ����
    acl::redis_client client(redis_addr, conn_timeout, rw_timeout);
    acl::redis cmd(&client);  // Redis ��� ��ü ����

    go[&]{
        while (true) {
            acl::socket_stream* conn = server.accept();
            if (conn) {
                go[=, &cmd] {  // Redis ��� ��ü ���� ����
                    char buf[256];
                    while (true) {
                        int ret = conn->read(buf, sizeof(buf), false);
                        if (ret <= 0) {  // ������ �б� ���� �Ǵ� ���� ���� ��
                            std::cerr << "Failed to read data from client or connection closed." << std::endl;
                            break;
                        }

                        buf[ret] = '\0';  // ���ڿ��� ó���ϱ� ���� null-terminator �߰�
                        std::cout << "Received from client: " << buf << std::endl;

                        // Ŭ���̾�Ʈ�κ��� ���� �����͸� Redis�� ����
                        const char* key = "client_input";
                        int ttl = 300;  // 5�� (300��)
                        set_with_ttl(cmd, key, buf, ttl);

                        if (conn->write(buf, ret) != ret) {
                            std::cerr << "Failed to send data to client." << std::endl;
                            break;
                        }
                    }
                    delete conn;  // ���� ���� �� ���� ��Ʈ�� ����
                };
            }
        }
    };

    acl::fiber::schedule();  // �ڷ�ƾ �����ٷ� ����
}

void run_tcp_coroutine_server() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;
    if (!server.open(addr)) {
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return;
    }

    std::cout << "Coroutine TCP Server is running on " << addr << std::endl;

    go[&]{
        while (true) {
            acl::socket_stream* conn = server.accept();
            if (conn) {
                go[=] {
                    char buf[256];
                    while (true) {
                        int ret = conn->read(buf, sizeof(buf), false);
                        if (ret <= 0) {  // ������ �б� ���� �Ǵ� ���� ���� ��
                            std::cerr << "Failed to read data from client or connection closed." << std::endl;
                            break;
                        }

                        buf[ret] = '\0';  // ���ڿ��� ó���ϱ� ���� null-terminator �߰�
                        std::cout << "Received from client: " << buf << std::endl;

                        if (conn->write(buf, ret) != ret) {
                            std::cerr << "Failed to send data to client." << std::endl;
                            break;
                        }
                    }
                    delete conn;  // ���� ���� �� ���� ��Ʈ�� ����
                };
            }
        }
    };

    acl::fiber::schedule();  // �ڷ�ƾ �����ٷ� ����
}
