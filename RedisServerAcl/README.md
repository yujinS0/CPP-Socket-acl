# Acl을 활용하여 Redis 사용하는 예제

## Acl Redis
* [Acl redis - One C++ redis client library in Acl](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md)
* ACL은 다양한 기능을 제공하는 C++ 라이브러리로, 네트워크 프로그래밍과 코루틴, **Redis**와 같은 데이터베이스와의 상호작용 등을 쉽게 할 수 있게 해줍니다.

## 실습 목표
1. Redis와 연결 설정: Redis 서버에 연결하고 기본 설정을 확인합니다.
2. String 자료형 실습: 키-값 쌍을 저장하고 TTL(Time To Live)을 설정하며 값을 조회합니다.
3. List 자료형 실습: 리스트에 값을 추가하고 특정 범위의 값을 조회합니다.
4. Set 자료형 실습: 집합(Set)에 값을 추가하고 조회합니다.
5. Hash 자료형 실습: 해시(Hash)에 필드와 값을 저장하고 조회합니다.
6. Sorted Set (ZSet) 자료형 실습: 정렬된 집합에 값과 점수를 저장하고 조회합니다.

## 코드 설명

### 1. Redis 연결 설정

#### `redisHandler.h`
```cpp
#include <iostream>
#include <acl_cpp/lib_acl.hpp>

// Redis 연결 설정
acl::redis_client* setup_redis();
```

#### `redisHandler.cpp`
```cpp
#include "redisHandler.h"
#include <iostream>

acl::redis_client* setup_redis() {
    acl::string redis_addr("127.0.0.1:6379");
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
```
- **`setup_redis()`**: Redis 서버에 연결하는 함수입니다. 연결에 성공하면 클라이언트를 반환하고, 실패하면 에러 메시지를 출력합니다.


<br>

### 2. String 자료형 실습

#### `redisHandler.h`
```cpp
void handle_string(acl::redis& redis);
void handle_get(acl::redis& redis);
```

#### `redisHandler.cpp`
```cpp
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
```
- **`handle_string()`**: 사용자가 입력한 키와 값을 Redis에 저장합니다. TTL을 설정하여 데이터의 만료 시간을 지정할 수 있습니다.
- **`handle_get()`**: 사용자가 입력한 키를 사용해 Redis에 저장된 값을 조회하고 출력합니다.

<br>


### 3. List 자료형 실습

#### `redisHandler.h`
```cpp
void handle_list_push(acl::redis_list& redis);
void handle_list_get(acl::redis_list& redis);
```

#### `redisHandler.cpp`
```cpp
void handle_list_push(acl::redis_list& redis) {
    std::string key, value;
    char action;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;
    std::cout << "왼쪽(L) 또는 오른쪽(R) 입력을 선택하세요: ";
    std::cin >> action;

    acl::string acl_key(key.c_str());

    while (true) {
        std::cout << "리스트에 추가할 값을 입력하세요 (종료하려면 q 입력): ";
        std::cin >> value;
        if (value == "q") break;

        acl::string acl_value(value.c_str());

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

void handle_list_get(acl::redis_list& redis) {
    std::string key;
    int start, end;

    std::cout << "key를 입력하세요: ";
    std::cin >> key;
    std::cout << "조회 시작 인덱스를 입력하세요: ";
    std::cin >> start;
    std::cout << "조회 종료 인덱스를 입력하세요: ";
    std::cin >> end;

    acl::string acl_key(key.c_str());
    std::vector<acl::string> list_values;

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
```
- **`handle_list_push()`**: 사용자가 입력한 값을 리스트의 왼쪽(LPUSH) 또는 오른쪽(RPUSH)에 추가합니다.
- **`handle_list_get()`**: 사용자가 지정한 범위 내에서 리스트의 값을 조회하고 출력합니다.

<br>


### 4. Set 자료형 실습

#### `redisHandler.h`
```cpp
void handle_set(acl::redis& redis);
```

#### `redisHandler.cpp`
```cpp
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
```
- **`handle_set()`**: 사용자가 입력한 값을 Redis의 Set에 추가합니다. `q`를 입력하면 종료됩니다.

<br>


### 5. Hash 자료형 실습

#### `redisHandler.h`
```cpp
void handle_hash(acl::redis& redis);
```

#### `redisHandler.cpp`
```cpp
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
```
- **`handle_hash()`**: 해시 자료형에 필드와 값을 저장합니다. 필드와 값을 반복적으로 입력할 수 있습니다.

<br>


### 6. Sorted Set (ZSet) 자료형 실습

#### `redisHandler.h`
```cpp
void handle_zset(acl::redis& redis);
```

#### `redisHandler.cpp`
```cpp
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
```
- **`handle_zset()`**: 정렬된 집합(ZSet)에 값과 점수를 추가합니다. 점수는 멤버의 순서를 결정하는데 사용됩니다.


<br>



## 코드 예제 실행

### `main.cpp`
```cpp
#include <iostream>
#include "redisHandler.h"

int main() {
    acl::acl_cpp_init();  // ACL 초기화

    acl::redis_client* client = setup_redis();
    if (!client) {
        return -1;
    }

    acl::redis redis;
    redis.set_client(client);

    while (true) {
        std::string command;
        std::cout << "\n[Redis Test with Acl] 명령어를 입력하세요 (set, get, list_push, list_get, hash, zset, exit): ";
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
```

- **`acl::acl_cpp_init()`**: ACL 라이브러리 초기화 함수입니다. 이 함수를 호출하여 ACL 내부의 여러 기능을 사용할 준비를 합니다.


이 코드를 통해 다양한 Redis 자료형을 연습하고, 데이터를 저장하고 조회하는 실습을 진행할 수 있습니다.

-------------------

아 외에도 아래와 같은 다양한 기능을 사용할 수 있습니다. 

- [cluster example for redis3.0+](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#22-redis-client-cluster-example-for-redis30)
- [Using redis client cluster in multi-threads](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#23-using-redis-client-cluster-in-multi-threads)
- [redis pipeline in multi-threads](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#24-use-redis-pipeline-in-multi-threads)




