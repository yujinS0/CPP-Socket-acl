#include "pch.h"
#include "tcpCoroutine.h"
#include "redisHandler.h"
#include "PacketDefinition.h"
#include "RoomManager.h"

void run_tcp_coroutine_server_with_redis_and_rooms() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;
    if (!server.open(addr)) {
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return;
    }

    std::cout << "Chat Server is running on " << addr << std::endl;

    // Redis ���� ����
    const char* redis_addr = "127.0.0.1:6379";
    int conn_timeout = 10;  // Redis ���� Ÿ�Ӿƿ� (��)
    int rw_timeout = 10;    // Redis �б�/���� Ÿ�Ӿƿ� (��)

    // Redis Ŭ���̾�Ʈ ����
    acl::redis_client client(redis_addr, conn_timeout, rw_timeout);
    acl::redis cmd(&client);  // Redis ��� ��ü ����

    RoomManager roomManager;  

    go[&]{
        while (true) {
            acl::socket_stream* conn = server.accept();
            if (conn) {
                go[=, &cmd, &roomManager] {  // Redis �� RoomManager ��ü ���� ����
                    char buf[256];
                    std::string currentUserID;
                    int currentRoomNumber = -1;

                    while (true) {
                        std::memset(buf, 0, sizeof(buf));

                        int ret = conn->read(buf, sizeof(buf), false);
                        if (ret <= 0) {  // ������ �б� ���� �Ǵ� ���� ���� ��
                            std::cerr << "Failed to read data from client or connection closed." << std::endl;
                            break;
                        }

                        PacketHeader header = PacketHeader::Deserialize(buf);

                        switch (header.Id) {
                        case PacketID::ReqLogin: {
                            LoginRequest loginRequest = LoginRequest::Deserialize(buf);

                            acl::string storedPassword;
                            bool userExists = cmd.get(loginRequest.UserID, storedPassword);

                            if (userExists && storedPassword == loginRequest.AuthToken) {
                                std::string successMessage = "Login Success!";
                                conn->write(successMessage.c_str(), successMessage.size());
                                currentUserID = loginRequest.UserID;  // ���� ����� ID ����
                            }
                            else {
                                std::string failureMessage = "Login Failed!";
                                conn->write(failureMessage.c_str(), failureMessage.size());
                            }
                            break;
                        }
                        case PacketID::ReqRoomEnter: {
                            RoomEnterRequest roomEnterRequest{};
                            roomEnterRequest = RoomEnterRequest::Deserialize(buf);
                            currentRoomNumber = roomEnterRequest.RoomNumber;
                            if (!currentUserID.empty()) {
                                roomManager.EnterRoom(currentRoomNumber, currentUserID, conn);
                                std::cout << "User " << currentUserID << " entered room " << currentRoomNumber << std::endl;
                            }
                            break;
                        }
                        case PacketID::ReqRoomChat: {
                            RoomChatRequest roomChatRequest{};
                            roomChatRequest = RoomChatRequest::Deserialize(buf);
                            if (currentRoomNumber != -1 && !currentUserID.empty()) {
                                roomManager.BroadcastMessage(currentRoomNumber, roomChatRequest.Message, currentUserID);
                                std::cout << "User " << currentUserID << " sent message: " << roomChatRequest.Message << std::endl;
                            }
                            break;
                        }
                        default:
                            std::cerr << "Unknown packet ID: " << static_cast<int>(header.Id) << std::endl;
                            break;
                        }
                    }
                    if (currentRoomNumber != -1 && !currentUserID.empty()) {
                        roomManager.LeaveRoom(currentRoomNumber, currentUserID);  // ���� ���� �� ������ �濡�� ����
                    }
                    delete conn;  // ���� ���� �� ���� ��Ʈ�� ����
                };
            }
        }
    };

    acl::fiber::schedule();  // �ڷ�ƾ �����ٷ� ����
}
