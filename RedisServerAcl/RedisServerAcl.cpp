#include <iostream>

#include "pch.h"
#include "redisHandler.h"
#include "redisHandler.h"
#include <iostream>
#include <vector>

static const int max_print_line = 50;


///
int main() {
    acl::acl_cpp_init();
    acl::log::stdout_open(true);

    acl::redis_client* client = setup_redis();
    if (!client) {
        return -1;
    }

    acl::redis redis;
    redis.set_client(client);

    while (true) {
        std::string command;
        std::cout << "\n명령어를 입력하세요 (set, get, list_push, list_get, hash, zset, exit): ";
        std::cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "set") {
            handle_string(redis);
        }
        else if (command == "get") {
            handle_get(redis);
        }
        else if (command == "list_push") {
            handle_list_push(redis);
        }
        else if (command == "list_get") {
            handle_list_get(redis);
        }
        else if (command == "hash") {
            handle_hash(redis);
        }
        else if (command == "zset") {
            handle_zset(redis);
        }
        else {
            std::cout << "지원하지 않는 명령어입니다." << std::endl;
        }
    }

    delete client;
    return 0;
}