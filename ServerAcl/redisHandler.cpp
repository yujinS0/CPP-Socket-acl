#include "redisHandler.h"
#include <iostream>

// 데이터를 Redis에 5분 동안 저장하는 함수
void set_with_ttl(acl::redis& cmd, const char* key, const char* value, int ttl) {
    // 키-값 쌍을 Redis에 저장하고 TTL 설정
    if (cmd.setex(key, value, ttl) == false) {
        std::cerr << "Failed to set key with TTL in Redis" << std::endl;
    }
    else {
        std::cout << "Data saved with " << ttl << " seconds TTL." << std::endl;
    }
}

// Redis에서 데이터를 조회하는 함수
void get_value(acl::redis& cmd, const char* key) {
    acl::string value;
    // Redis에서 값 조회
    if (cmd.get(key, value) == false) {
        std::cerr << "Failed to get value from Redis" << std::endl;
    }
    else {
        std::cout << "Value for key '" << key << "': " << value.c_str() << std::endl;
    }
}
