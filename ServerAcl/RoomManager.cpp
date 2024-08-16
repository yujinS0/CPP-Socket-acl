#include "RoomManager.h"
#include "PacketDefinition.h"
#define _CRT_SECURE_NO_WARNINGS

void RoomManager::EnterRoom(int roomNumber, const std::string& userID, acl::socket_stream* conn) {
    // 현재 방에 유저 수 확인
    if (rooms_[roomNumber].size() >= 2) {
        std::string message = "Room is full!";
        conn->write(message.c_str(), message.size());
        return;
    }

    // 방에 유저 추가
    rooms_[roomNumber][userID] = conn;

    // 방에 있는 유저 목록을 모든 클라이언트에게 브로드캐스트
    SendUserListToAll(roomNumber);

    // 모든 유저에게 새로운 유저가 입장했음을 알림
    std::string joinMessage = userID + " has entered the room.";
    BroadcastMessage(roomNumber, joinMessage, "System");
}

void RoomManager::LeaveRoom(int roomNumber, const std::string& userID) {
    // 방에서 유저 제거
    rooms_[roomNumber].erase(userID);

    // 모든 유저에게 유저가 방을 나갔음을 알림
    std::string leaveMessage = userID + " has left the room.";
    BroadcastMessage(roomNumber, leaveMessage, "System");

    // 방이 비었을 경우 해당 방 삭제
    if (rooms_[roomNumber].empty()) {
        rooms_.erase(roomNumber);
    }
    else {
        // 방에 남아있는 유저들에게 유저 목록을 브로드캐스트
        SendUserListToAll(roomNumber);
    }
}

void RoomManager::SendUserListToAll(int roomNumber) {
    if (rooms_.find(roomNumber) == rooms_.end()) {
        return;
    }

    // 유저 목록 구성
    std::string userList = "User list in room: ";
    for (const auto& pair : rooms_[roomNumber]) {
        userList += pair.first + " ";
    }

    // 유저 목록을 방에 있는 모든 유저에게 브로드캐스트
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
        const std::string& receiverID = pair.first; // 수신자 ID

        RoomChatNotification notification;
        notification.TotalSize = sizeof(RoomChatNotification);
        notification.Id = PacketID::NtfRoomChat;
        std::strncpy(notification.UserID, senderID.c_str(), sizeof(notification.UserID) - 1);
        std::strncpy(notification.Message, message.c_str(), sizeof(notification.Message) - 1);

        char buffer[sizeof(RoomChatNotification)];
        notification.Serialize(buffer);

        // 디버깅 로그 추가: 각 클라이언트에게 브로드캐스트하는 메시지와 수신자 ID 출력
        std::cout << "Broadcasting Message: [" << senderID << "] " << message
            << " to client [" << receiverID << "]." << std::endl;

        // 메시지의 길이와 버퍼의 크기 확인
        std::cout << "Message length: " << message.length() << ", Buffer size: " << sizeof(buffer) << std::endl;


        conn->write(buffer, sizeof(buffer));
    }
}
