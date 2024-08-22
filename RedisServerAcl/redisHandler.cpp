#include "redisHandler.h"
#include "pch.h"
#include "User.h"
#include <limits>  // 이 헤더를 포함해야 std::numeric_limits 사용 가능
#include <map>

// redis 연결 설정
// redis_client 반환하는 함수
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

// redis return 반환하는 함수 (주소 입력받기)
acl::redis* setup_redis(const char* redis_addr) {
    // Redis 클라이언트 생성
    acl::redis_client* client = new acl::redis_client(redis_addr, 10, 10);
    acl::redis* redis = new acl::redis();
    redis->set_client(client);  // redis_client를 redis에 설정

    // 연결 테스트
    if (!redis->ping()) {
        std::cerr << "Redis 연결 실패!" << std::endl;
        delete redis;
        delete client;
        return nullptr;
    }

    std::cout << "Redis 연결 성공!" << std::endl;
    return redis;  // 설정된 redis 객체 반환
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

        while (true) {
            std::cout << "해당 값의 점수를 입력하세요: ";
            std::cin >> score;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cerr << "유효한 숫자를 입력하세요!" << std::endl;
            }
            else {
                break;  // 유효한 숫자가 입력된 경우 루프를 빠져나감
            }
        }

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

// JSON 관련 핸들러 함수들
void handle_set_json(acl::redis_client& client) {
    std::string key, name, email;
    int age;

    std::cout << "Enter key: ";
    std::cin >> key;
    std::cout << "Enter name: ";
    std::cin >> name;
    std::cout << "Enter age: ";
    std::cin >> age;
    std::cout << "Enter email: ";
    std::cin >> email;

    User user = { name, age, email };

    if (set_json_data(client, key, user)) {
        std::cout << "User data saved successfully in Redis!" << std::endl;
    }
    else {
        std::cerr << "Failed to save user data in Redis." << std::endl;
    }
}

void handle_get_json(acl::redis_client& client) {
    std::string key;
    std::cout << "Enter key: ";
    std::cin >> key;

    try {
        User user = json_to_user(client, key);
        std::cout << "Name: " << user.name
            << ", Age: " << user.age
            << ", Email: " << user.email << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

// Redis에 JSON 데이터를 저장하는 함수
bool set_json_data(acl::redis_client& client, const std::string& key, const User& user) {
    acl::json json;
    user_to_json(user, json);

    acl::string json_str;
    json.build_json(json_str);

    acl::redis_command cmd(&client);
    std::vector<const char*> argv = { "JSON.SET", key.c_str(), "$", json_str.c_str() };
    std::vector<size_t> lens = { strlen("JSON.SET"), key.size(), strlen("$"), json_str.size() };

    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());
    return result && result->get_type() == acl::REDIS_RESULT_STATUS;
}

// Redis에서 각 필드를 개별적으로 가져오는 함수
std::string get_json_field(acl::redis_client& client, const std::string& key, const std::string& field) {
    acl::redis_command cmd(&client);
    std::vector<const char*> argv = { "JSON.GET", key.c_str(), field.c_str() };
    std::vector<size_t> lens = { strlen("JSON.GET"), key.size(), field.size() };

    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());

    if (!result || result->get_type() != acl::REDIS_RESULT_STRING) {
        throw std::runtime_error("Failed to retrieve JSON field from Redis");
    }
    return result->get(0);
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