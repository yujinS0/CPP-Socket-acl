#include "pch.h"
#include "tcpCoroutine.h"
#include "redisHandler.h"
#include "PacketDefinition.h"

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

                        PacketHeader header = PacketHeader::Deserialize(buf);

                        if (header.Id == PacketID::ReqLogin) {
                            LoginRequest loginRequest = LoginRequest::Deserialize(buf);

                            acl::string storedPassword;
                            bool userExists = cmd.get(loginRequest.UserID, storedPassword);

                            if (userExists && storedPassword == loginRequest.AuthToken) {
                                std::string successMessage = "Login Success!";
                                conn->write(successMessage.c_str(), successMessage.size());
                            }
                            else {
                                std::string failureMessage = "Login Failed!";
                                conn->write(failureMessage.c_str(), failureMessage.size());
                            }
                        }
                    }
                    delete conn;  // ���� ���� �� ���� ��Ʈ�� ����
                };
            }
        }
    };

    acl::fiber::schedule();  // �ڷ�ƾ �����ٷ� ����
}
