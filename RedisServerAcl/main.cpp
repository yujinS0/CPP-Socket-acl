#include "pch.h"
#include "redisHandler.h"

int main() {
    acl::acl_cpp_init();
    acl::log::stdout_open(true);

    /*acl::redis_client* client = setup_redis();
    if (!client) {
        return -1;
    }

    acl::redis redis;
    redis.set_client(client);*/
    
    
    // Redis 클라이언트 설정
    //const char* redis_addr = "127.0.0.1:6389";
    //acl::redis_client client(redis_addr, 10, 10);
    //acl::redis redis;
    //redis.set_client(&client);  // redis_client를 redis에 설정


    // Redis 설정 함수 호출
    const char* redis_addr = "127.0.0.1:6389";  // RedisJSON 서버 주소
    acl::redis* redis = setup_redis(redis_addr);
    if (!redis) {
        return -1;  // Redis 연결 실패 시 종료
    }

    acl::redis_client* client = redis->get_client();  // redis_client 객체 가져오기


    while (true) {
        std::string command;
        std::cout << "\n[Redis Test with Acl] 명령어를 입력하세요 (set, get, list_push, list_get, hash, zset, set_json, get_json, exit): ";
        std::cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "set") {
            handle_string(*redis);
        }
        else if (command == "get") {
            handle_get(*redis);
        }
        else if (command == "list_push") {
            handle_list_push(*redis);
        }
        else if (command == "list_get") {
            handle_list_get(*redis);
        }
        else if (command == "hash") {
            handle_hash(*redis);
        }
        else if (command == "zset") {
            handle_zset(*redis);
        }
        else if (command == "set_json") {
            handle_set_json(*client); 
        }
        else if (command == "get_json") {
            handle_get_json(*client);
        }
        else {
            std::cout << "지원하지 않는 명령어입니다." << std::endl;
        }
    }

    return 0;
}