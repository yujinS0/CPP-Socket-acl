#include "redisHandler.h"
#include <iostream>

// �����͸� Redis�� 5�� ���� �����ϴ� �Լ�
void set_with_ttl(acl::redis& cmd, const char* key, const char* value, int ttl) {
    // Ű-�� ���� Redis�� �����ϰ� TTL ����
    if (cmd.setex(key, value, ttl) == false) {
        std::cerr << "Failed to set key with TTL in Redis" << std::endl;
    }
    else {
        std::cout << "Data saved with " << ttl << " seconds TTL." << std::endl;
    }
}

// Redis���� �����͸� ��ȸ�ϴ� �Լ�
void get_value(acl::redis& cmd, const char* key) {
    acl::string value;
    // Redis���� �� ��ȸ
    if (cmd.get(key, value) == false) {
        std::cerr << "Failed to get value from Redis" << std::endl;
    }
    else {
        std::cout << "Value for key '" << key << "': " << value.c_str() << std::endl;
    }
}
