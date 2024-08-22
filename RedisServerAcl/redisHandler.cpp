#include "redisHandler.h"
#include "pch.h"

// Redis 연결 설정
acl::redis_client* setup_redis() {
    acl::string redis_addr("127.0.0.1:6389"); // RedisJSON 모듈(redislabs/rejson:latest)
    //acl::string redis_addr("127.0.0.1:6379"); // 일반 Redis
    acl::redis_client* client = new acl::redis_client(redis_addr.c_str(), 10, 10);

    acl::redis redis;
    redis.set_client(client);

    if (redis.ping() == false) {
        std::cerr << "Redis 연결 실패!" << std::endl;
        delete client;
        return nullptr;
    }

    std::cout << "Redis 연결 성공!" << std::endl;
    return client;
}

void handle_string(acl::redis& redis) {
    std::string key, value;
    int ttl = -1;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;
    std::cout << "value를 입력하세요: ";
    std::cin >> value;
    std::cout << "TTL(초 단위로 입력, 기본값 -1: 만료되지 않음): ";
    std::cin >> ttl;

    acl::string acl_key(key.c_str());
    acl::string acl_value(value.c_str());

    redis.set(acl_key, acl_value);
    if (ttl > 0) {
        redis.expire(acl_key, ttl);
    }
    std::cout << "String 저장 완료!" << std::endl;
}

void handle_get(acl::redis& redis) {
    std::string key;
    acl::string value;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;

    acl::string acl_key(key.c_str());

    if (redis.get(acl_key, value)) {
        if (value.empty()) {
            std::cout << "key '" << acl_key.c_str() << "'에 해당하는 값이 없습니다." << std::endl;
        }
        else {
            std::cout << "key: " << acl_key.c_str() << ", value: " << value.c_str() << std::endl;
        }
    }
    else {
        std::cerr << "Redis에서 key '" << acl_key.c_str() << "'를 가져오는 중 오류가 발생했습니다." << std::endl;
    }
}

void handle_list_get(acl::redis_list& redis) {
    std::string key;
    int start, end;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;
    std::cout << "조회 시작 인덱스를 입력하세요: ";
    std::cin >> start;
    std::cout << "조회 종료 인덱스를 입력하세요: ";
    std::cin >> end;

    acl::string acl_key(key.c_str());  // 안전한 방식으로 std::string -> acl::string 변환
    std::vector<acl::string> list_values;  // 안전한 STL 컨테이너 사용

    redis.clear();
    if (redis.lrange(acl_key.c_str(), start, end, &list_values)) {
        if (list_values.empty()) {
            std::cout << "리스트가 비어있습니다." << std::endl;
        }
        else {
            std::cout << "리스트 값들: " << std::endl;
            for (size_t i = 0; i < list_values.size(); ++i) {
                std::cout << "Index " << start + i << ": " << list_values[i].c_str() << std::endl;
            }
        }
    }
    else {
        std::cerr << "Redis에서 리스트 '" << acl_key.c_str() << "'를 조회하는 중 오류가 발생했습니다." << std::endl;
    }
}

void handle_list_push(acl::redis_list& redis) {
    std::string key, value;
    char action;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;
    std::cout << "왼쪽(L) 또는 오른쪽(R) 입력을 선택하세요: ";
    std::cin >> action;

    acl::string acl_key(key.c_str());  // std::string -> acl::string 변환

    while (true) {
        std::cout << "리스트에 추가할 값을 입력하세요 (종료하려면 q 입력): ";
        std::cin >> value;
        if (value == "q") break;

        acl::string acl_value(value.c_str());  // std::string -> acl::string 변환

        bool success = false;
        if (action == 'L' || action == 'l') {
            success = redis.lpush(acl_key, acl_value, NULL) >= 0;
        }
        else if (action == 'R' || action == 'r') {
            success = redis.rpush(acl_key, acl_value, NULL) >= 0;
        }

        if (success) {
            std::cout << "리스트에 값이 성공적으로 추가되었습니다: " << acl_value.c_str() << std::endl;
        }
        else {
            std::cerr << "리스트에 값을 추가하는 중 오류가 발생했습니다." << std::endl;
        }
    }
}


void handle_set(acl::redis& redis) {
    std::string key, value;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;

    acl::string acl_key(key.c_str());

    while (true) {
        std::cout << "집합에 추가할 값을 입력하세요 (종료하려면 q 입력): ";
        std::cin >> value;
        if (value == "q") break;

        acl::string acl_value(value.c_str());
        redis.sadd(acl_key, acl_value);
    }

    std::cout << "Set 저장 완료!" << std::endl;
}

void handle_hash(acl::redis& redis) {
    std::string key, field, value;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;

    acl::string acl_key(key.c_str());

    while (true) {
        std::cout << "해시 필드를 입력하세요 (종료하려면 q 입력): ";
        std::cin >> field;
        if (field == "q") break;

        std::cout << "해시 필드의 값을 입력하세요: ";
        std::cin >> value;

        acl::string acl_field(field.c_str());
        acl::string acl_value(value.c_str());

        redis.hset(acl_key, acl_field, acl_value);
    }

    std::cout << "Hash 저장 완료!" << std::endl;
}

void handle_zset(acl::redis& redis) {
    std::string key, value;
    std::map<acl::string, double> members;
    double score;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;

    acl::string acl_key(key.c_str());

    while (true) {
        std::cout << "Sorted Set에 추가할 값을 입력하세요 (종료하려면 q 입력): ";
        std::cin >> value;
        if (value == "q") break;

        std::cout << "해당 값의 점수를 입력하세요: ";
        std::cin >> score;

        acl::string acl_value(value.c_str());
        members[acl_value] = score;
    }

    if (!members.empty()) {
        redis.zadd(acl_key, members);
        std::cout << "Sorted Set 저장 완료!" << std::endl;
    }
    else {
        std::cout << "저장할 멤버가 없습니다." << std::endl;
    }
}




// 기본 실습 코드
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
    if (cmd.get(key, value)) {
        if (value.empty()) {
            std::cerr << "key '" << key << "'에 해당하는 값이 존재하지 않습니다." << std::endl;
        }
        else {
            std::cout << "Value for key '" << key << "': " << value.c_str() << std::endl;
        }
    }
    else {
        std::cerr << "Redis에서 key '" << key << "'를 가져오는 중 오류가 발생했습니다." << std::endl;
    }
}