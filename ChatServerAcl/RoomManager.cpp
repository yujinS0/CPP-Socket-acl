#include "RoomManager.h"
#include "PacketDefinition.h"
#define _CRT_SECURE_NO_WARNINGS

void RoomManager::EnterRoom(int roomNumber, const std::string& userID, acl::socket_stream* conn) {
    // ���� �濡 ���� �� Ȯ��
    if (rooms_[roomNumber].size() >= 2) {
        std::string message = "Room is full!";
        conn->write(message.c_str(), message.size());
        return;
    }

    // �濡 ���� �߰�
    rooms_[roomNumber][userID] = conn;

    // �濡 �ִ� ���� ����� ��� Ŭ���̾�Ʈ���� ��ε�ĳ��Ʈ
    SendUserListToAll(roomNumber);

    // ��� �������� ���ο� ������ ���������� �˸�
    std::string joinMessage = userID + " has entered the room.";
    BroadcastMessage(roomNumber, joinMessage, "System");
}

void RoomManager::LeaveRoom(int roomNumber, const std::string& userID) {
    // �濡�� ���� ����
    rooms_[roomNumber].erase(userID);

    // ��� �������� ������ ���� �������� �˸�
    std::string leaveMessage = userID + " has left the room.";
    BroadcastMessage(roomNumber, leaveMessage, "System");

    // ���� ����� ��� �ش� �� ����
    if (rooms_[roomNumber].empty()) {
        rooms_.erase(roomNumber);
    }
    else {
        // �濡 �����ִ� �����鿡�� ���� ����� ��ε�ĳ��Ʈ
        SendUserListToAll(roomNumber);
    }
}

void RoomManager::SendUserListToAll(int roomNumber) {
    if (rooms_.find(roomNumber) == rooms_.end()) {
        return;
    }

    // ���� ��Ͽ��� �ִ� �� ���� ���� ID�� �����ɴϴ�.
    std::string userID1 = "";
    std::string userID2 = "";

    int userIndex = 0;
    for (const auto& pair : rooms_[roomNumber]) {
        if (userIndex == 0) {
            userID1 = pair.first;
        }
        else if (userIndex == 1) {
            userID2 = pair.first;
            break; // �� ������� ����
        }
        userIndex++;
    }

    // UserListNotification ��Ŷ ����
    UserListNotification notification;
    notification.TotalSize = sizeof(UserListNotification);
    notification.Id = PacketID::NtfUserList;
    std::strncpy(notification.UserID1, userID1.c_str(), sizeof(notification.UserID1) - 1);
    std::strncpy(notification.UserID2, userID2.c_str(), sizeof(notification.UserID2) - 1);

    // ��Ŷ�� ����ȭ
    char buffer[sizeof(UserListNotification)];
    notification.Serialize(buffer);

    // ��� Ŭ���̾�Ʈ���� ��ε�ĳ��Ʈ
    std::cout << "Broadcasting User List: " << userID1 << " " << userID2 << " to all clients." << std::endl;
    Broadcast(roomNumber, buffer, sizeof(buffer));
}

void RoomManager::BroadcastMessage(int roomNumber, const std::string& message, const std::string& senderID) {
    RoomChatNotification notification;
    notification.TotalSize = sizeof(RoomChatNotification);
    notification.Id = PacketID::NtfRoomChat;
    std::strncpy(notification.UserID, senderID.c_str(), sizeof(notification.UserID) - 1);
    std::strncpy(notification.Message, message.c_str(), sizeof(notification.Message) - 1);

    // ��Ŷ�� ����ȭ
    char buffer[sizeof(RoomChatNotification)];
    notification.Serialize(buffer);

    // ����� �α� �߰�
    std::cout << "Broadcasting Message: [" << senderID << "] " << message << " to all clients." << std::endl;

    // ��� Ŭ���̾�Ʈ���� ��ε�ĳ��Ʈ
    Broadcast(roomNumber, buffer, sizeof(buffer));
}

void RoomManager::Broadcast(int roomNumber, const char* buffer, size_t bufferSize) {
    if (rooms_.find(roomNumber) == rooms_.end()) {
        return;
    }

    // �濡 �ִ� ��� �������� ��Ŷ �����͸� ����
    for (const auto& pair : rooms_[roomNumber]) {
        acl::socket_stream* conn = pair.second;
        const std::string& receiverID = pair.first; // ������ ID

        // ����� �α� �߰�: �� Ŭ���̾�Ʈ���� ������ ������ ���
        std::cout << "Broadcasting data to client [" << receiverID << "]." << std::endl;

        conn->write(buffer, bufferSize);
    }
}
