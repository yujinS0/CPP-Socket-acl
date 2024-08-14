#pragma once
#include <cstdint>
#include <cstring>

enum class PacketID : uint16_t {
    ReqLogin = 1002,
    ResLogin = 1003,
    ReqRoomEnter = 1021,
    ResRoomEnter = 1022,
    ReqRoomChat = 1026,
    NtfRoomChat = 1027,
};

struct PacketHeader {
    uint16_t TotalSize;
    PacketID Id;
    uint8_t Type;

    void Serialize(char* buffer) const;
    static PacketHeader Deserialize(const char* buffer);
};

struct LoginRequest : public PacketHeader {
    char UserID[32];
    char AuthToken[32];

    void Serialize(char* buffer) const;
    static LoginRequest Deserialize(const char* buffer);
};
struct RoomEnterRequest : public PacketHeader {
    int RoomNumber;

    void Serialize(char* buffer) const;
    static RoomEnterRequest Deserialize(const char* buffer);
};

struct RoomChatRequest : public PacketHeader {
    char Message[256];

    void Serialize(char* buffer) const;
    static RoomChatRequest Deserialize(const char* buffer);
};

struct RoomChatNotification : public PacketHeader {
    char UserID[32];
    char Message[256];

    void Serialize(char* buffer) const;
    static RoomChatNotification Deserialize(const char* buffer);
};