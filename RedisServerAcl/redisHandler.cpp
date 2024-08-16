#include "redisHandler.h"
#include <iostream>

void set_with_ttl(acl::redis& cmd, const char* key, const char* value, int ttl) {
    if (cmd.setex(key, value, ttl) == false) {
        std::cerr << "Failed to set key with TTL in Redis" << std::endl;
    }
    else {
        std::cout << "Data saved with " << ttl << " seconds TTL." << std::endl;
    }
}


void get_value(acl::redis& cmd, const char* key) {
    acl::string value;
    if (cmd.get(key, value) == false) {
        std::cerr << "Failed to get value from Redis" << std::endl;
    }
    else {
        std::cout << "Value for key '" << key << "': " << value.c_str() << std::endl;
    }
}
