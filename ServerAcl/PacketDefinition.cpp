#include "PacketDefinition.h"

void PacketHeader::Serialize(char* buffer) const {
    std::memcpy(buffer, &TotalSize, sizeof(TotalSize));
    std::memcpy(buffer + sizeof(TotalSize), &Id, sizeof(Id));
    std::memcpy(buffer + sizeof(TotalSize) + sizeof(Id), &Type, sizeof(Type));
}

PacketHeader PacketHeader::Deserialize(const char* buffer) {
    PacketHeader header;
    std::memcpy(&header.TotalSize, buffer, sizeof(header.TotalSize));
    std::memcpy(&header.Id, buffer + sizeof(header.TotalSize), sizeof(header.Id));
    std::memcpy(&header.Type, buffer + sizeof(header.TotalSize) + sizeof(header.Id), sizeof(header.Type));
    return header;
}

void LoginRequest::Serialize(char* buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer + sizeof(PacketHeader), UserID, sizeof(UserID));
    std::memcpy(buffer + sizeof(PacketHeader) + sizeof(UserID), AuthToken, sizeof(AuthToken));
}

LoginRequest LoginRequest::Deserialize(const char* buffer) {
    LoginRequest request;
    request.TotalSize = *(uint16_t*)buffer;
    request.Id = *(PacketID*)(buffer + sizeof(request.TotalSize));
    request.Type = buffer[sizeof(request.TotalSize) + sizeof(request.Id)];
    std::memcpy(request.UserID, buffer + sizeof(PacketHeader), sizeof(request.UserID));
    std::memcpy(request.AuthToken, buffer + sizeof(PacketHeader) + sizeof(request.UserID), sizeof(request.AuthToken));
    return request;
}

// RoomEnterRequest
void RoomEnterRequest::Serialize(char* buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer + sizeof(PacketHeader), &RoomNumber, sizeof(RoomNumber));
}

RoomEnterRequest RoomEnterRequest::Deserialize(const char* buffer) {
    RoomEnterRequest request;
    request.TotalSize = *(uint16_t*)buffer;
    request.Id = *(PacketID*)(buffer + sizeof(request.TotalSize));
    request.Type = buffer[sizeof(request.TotalSize) + sizeof(request.Id)];
    std::memcpy(&request.RoomNumber, buffer + sizeof(PacketHeader), sizeof(request.RoomNumber));
    return request;
}

// RoomChatRequest
void RoomChatRequest::Serialize(char* buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer + sizeof(PacketHeader), Message, sizeof(Message));
}

RoomChatRequest RoomChatRequest::Deserialize(const char* buffer) {
    RoomChatRequest request;
    request.TotalSize = *(uint16_t*)buffer;
    request.Id = *(PacketID*)(buffer + sizeof(request.TotalSize));
    request.Type = buffer[sizeof(request.TotalSize) + sizeof(request.Id)];
    std::memcpy(request.Message, buffer + sizeof(PacketHeader), sizeof(request.Message));
    return request;
}

// RoomChatNotification
void RoomChatNotification::Serialize(char* buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer + sizeof(PacketHeader), UserID, sizeof(UserID));
    std::memcpy(buffer + sizeof(PacketHeader) + sizeof(UserID), Message, sizeof(Message));
}

RoomChatNotification RoomChatNotification::Deserialize(const char* buffer) {
    RoomChatNotification notification;
    notification.TotalSize = *(uint16_t*)buffer;
    notification.Id = *(PacketID*)(buffer + sizeof(notification.TotalSize));
    notification.Type = buffer[sizeof(notification.TotalSize) + sizeof(notification.Id)];
    std::memcpy(notification.UserID, buffer + sizeof(PacketHeader), sizeof(notification.UserID));
    std::memcpy(notification.Message, buffer + sizeof(PacketHeader) + sizeof(notification.UserID), sizeof(notification.Message));
    return notification;
}