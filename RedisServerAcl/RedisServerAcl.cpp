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
        std::cout << "\n명령어를 입력하세요 (set, get, list_create, list_get, hash, zset, exit): ";
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
        else if (command == "list_create") {
            handle_list_create(redis);
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


///////////////////////////////////////////////
//int main() {
//    //std::cout << "__cplusplus value: " << __cplusplus << std::endl; // C++ 버전 확인용
//    SetConsoleOutputCP(CP_UTF8); // 콘솔의 코드 페이지를 UTF-8로 설정
//
//    acl::acl_cpp_init();  // ACL 초기화
//
//    // Redis 서버의 주소와 포트 설정
//    const char* redis_addr = "127.0.0.1:6379";
//    int conn_timeout = 10;  // 연결 타임아웃 (초)
//    int rw_timeout = 10;    // 읽기/쓰기 타임아웃 (초)
//
//    // Redis 클라이언트 생성
//    acl::redis_client client(redis_addr, conn_timeout, rw_timeout);
//    acl::redis cmd(&client);  // Redis 명령 객체 생성
//
//    const char* key = "my_key";
//    const char* value = "my_value";
//    int ttl = 300;  // 5분 (300초)
//
//    // 데이터를 Redis에 5분 동안 저장
//    set_with_ttl(cmd, key, value, ttl);
//
//    // 저장된 데이터를 Redis에서 조회
//    get_value(cmd, key);
//
//
//    return 0;
//}
