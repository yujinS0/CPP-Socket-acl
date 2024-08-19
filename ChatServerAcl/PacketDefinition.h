#pragma once
#include <cstdint>
#include <cstring>

enum class PacketID : uint16_t {
    ReqLogin = 1002,
    ResLogin = 1003,
    ReqRoomEnter = 1021,
    ResRoomEnter = 1022,
    NtfUserList = 2024,
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

struct UserListNotification : public PacketHeader {
    char UserID1[32];  // 첫 번째 유저 ID
    char UserID2[32];  // 두 번째 유저 ID (없으면 빈 문자열)

    void Serialize(char* buffer) const;
    static UserListNotification Deserialize(const char* buffer);
};


struct RoomChatRequest : public PacketHeader {
    char Message[256];

    RoomChatRequest() : PacketHeader() {
        std::memset(Message, 0, sizeof(Message));
    }

    void Serialize(char* buffer) const;
    static RoomChatRequest Deserialize(const char* buffer);
};

struct RoomChatNotification : public PacketHeader {
    char UserID[32];
    char Message[256];

    RoomChatNotification() : PacketHeader() {
        std::memset(UserID, 0, sizeof(UserID));
        std::memset(Message, 0, sizeof(Message));
    }

    void Serialize(char* buffer) const;
    static RoomChatNotification Deserialize(const char* buffer);
};