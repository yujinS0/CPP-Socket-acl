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

    void Serialize(char* buffer) const;
    static PacketHeader Deserialize(const char* buffer);
};

struct LoginRequest : public PacketHeader {
    char UserID[32];
    char AuthToken[32];

    void Serialize(char* buffer) const;
    static LoginRequest Deserialize(const char* buffer);
};