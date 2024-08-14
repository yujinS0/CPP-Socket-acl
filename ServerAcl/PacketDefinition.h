#pragma once
#include <cstdint>
#include <cstring>

enum class PacketID : uint16_t {
    ReqLogin = 1002,
    ResLogin = 1003,
};

struct PacketHeader {
    uint16_t TotalSize;
    PacketID Id;
    uint8_t Type;

    void Serialize(char* buffer) const {
        std::memcpy(buffer, &TotalSize, sizeof(TotalSize));
        std::memcpy(buffer + sizeof(TotalSize), &Id, sizeof(Id));
        std::memcpy(buffer + sizeof(TotalSize) + sizeof(Id), &Type, sizeof(Type));
    }

    static PacketHeader Deserialize(const char* buffer) {
        PacketHeader header;
        std::memcpy(&header.TotalSize, buffer, sizeof(header.TotalSize));
        std::memcpy(&header.Id, buffer + sizeof(header.TotalSize), sizeof(header.Id));
        std::memcpy(&header.Type, buffer + sizeof(header.TotalSize) + sizeof(header.Id), sizeof(header.Type));
        return header;
    }
};

struct LoginRequest : public PacketHeader {
    char UserID[32];
    char AuthToken[32];

    void Serialize(char* buffer) const {
        PacketHeader::Serialize(buffer);
        std::memcpy(buffer + sizeof(PacketHeader), UserID, sizeof(UserID));
        std::memcpy(buffer + sizeof(PacketHeader) + sizeof(UserID), AuthToken, sizeof(AuthToken));
    }

    static LoginRequest Deserialize(const char* buffer) {
        LoginRequest request;
        request.TotalSize = *(uint16_t*)buffer;
        request.Id = *(PacketID*)(buffer + sizeof(request.TotalSize));
        request.Type = buffer[sizeof(request.TotalSize) + sizeof(request.Id)];
        std::memcpy(request.UserID, buffer + sizeof(PacketHeader), sizeof(request.UserID));
        std::memcpy(request.AuthToken, buffer + sizeof(PacketHeader) + sizeof(request.UserID), sizeof(request.AuthToken));
        return request;
    }
};