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

    // ���� ��� ����
    std::string userList = "User list in room: ";
    for (const auto& pair : rooms_[roomNumber]) {
        userList += pair.first + " ";
    }

    // ���� ����� �濡 �ִ� ��� �������� ��ε�ĳ��Ʈ
    for (const auto& pair : rooms_[roomNumber]) {
        acl::socket_stream* conn = pair.second;

        RoomChatNotification notification;
        notification.TotalSize = sizeof(RoomChatNotification);
        notification.Id = PacketID::NtfRoomChat;
        std::strncpy(notification.UserID, "System", sizeof(notification.UserID) - 1);
        std::strncpy(notification.Message, userList.c_str(), sizeof(notification.Message) - 1);

        char buffer[sizeof(RoomChatNotification)];
        notification.Serialize(buffer);

        std::cout << "Broadcasting User List: " << userList << " to client [" << pair.first << "]." << std::endl;

        conn->write(buffer, sizeof(buffer));
    }
}

void RoomManager::BroadcastMessage(int roomNumber, const std::string& message, const std::string& senderID) {
    for (const auto& pair : rooms_[roomNumber]) {
        acl::socket_stream* conn = pair.second;
        const std::string& receiverID = pair.first; // ������ ID

        RoomChatNotification notification;
        notification.TotalSize = sizeof(RoomChatNotification);
        notification.Id = PacketID::NtfRoomChat;
        std::strncpy(notification.UserID, senderID.c_str(), sizeof(notification.UserID) - 1);
        std::strncpy(notification.Message, message.c_str(), sizeof(notification.Message) - 1);

        char buffer[sizeof(RoomChatNotification)];
        notification.Serialize(buffer);

        // ����� �α� �߰�: �� Ŭ���̾�Ʈ���� ��ε�ĳ��Ʈ�ϴ� �޽����� ������ ID ���
        std::cout << "Broadcasting Message: [" << senderID << "] " << message
            << " to client [" << receiverID << "]." << std::endl;

        // �޽����� ���̿� ������ ũ�� Ȯ��
        std::cout << "Message length: " << message.length() << ", Buffer size: " << sizeof(buffer) << std::endl;


        conn->write(buffer, sizeof(buffer));
    }
}
