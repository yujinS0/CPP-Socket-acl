#include "RoomManager.h"
#include "PacketDefinition.h"
#define _CRT_SECURE_NO_WARNINGS

void RoomManager::EnterRoom(int roomNumber, const std::string& userID, acl::socket_stream* conn) {
    rooms_[roomNumber][userID] = conn;

    // Broadcast to all users in the room about the new user
    std::string message = userID + " has entered the room.";
    BroadcastMessage(roomNumber, message, "System");
}

void RoomManager::LeaveRoom(int roomNumber, const std::string& userID) {
    rooms_[roomNumber].erase(userID);

    // Broadcast to all users in the room about the user leaving
    std::string message = userID + " has left the room.";
    BroadcastMessage(roomNumber, message, "System");

    if (rooms_[roomNumber].empty()) {
        rooms_.erase(roomNumber);
    }
}

void RoomManager::BroadcastMessage(int roomNumber, const std::string& message, const std::string& senderID) {
    for (const auto& pair : rooms_[roomNumber]) {
        acl::socket_stream* conn = pair.second;

        RoomChatNotification notification;
        notification.TotalSize = sizeof(RoomChatNotification);
        notification.Id = PacketID::NtfRoomChat;
        std::strncpy(notification.UserID, senderID.c_str(), sizeof(notification.UserID) - 1);
        std::strncpy(notification.Message, message.c_str(), sizeof(notification.Message) - 1);

        char buffer[sizeof(RoomChatNotification)];
        notification.Serialize(buffer);
        conn->write(buffer, sizeof(buffer));
    }
}
