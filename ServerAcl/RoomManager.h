#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "acl_cpp/lib_acl.hpp"

class RoomManager {
public:
    void EnterRoom(int roomNumber, const std::string& userID, acl::socket_stream* conn);
    void LeaveRoom(int roomNumber, const std::string& userID);
    void BroadcastMessage(int roomNumber, const std::string& message, const std::string& senderID);

private:
    std::unordered_map<int, std::unordered_map<std::string, acl::socket_stream*>> rooms_;
};
