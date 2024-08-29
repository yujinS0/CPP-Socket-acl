#pragma once
#include <string>
#include <acl_cpp/lib_acl.hpp>

// User ����ü ����
struct User {
    std::string name;
    int age;
    std::string email;
};

// JSON ��ȯ ���� �Լ� ����
void user_to_json(const User& user, acl::json& json);
[[nodiscard]] User json_to_user(acl::redis_client& client, std::string_view key);
