# Acl을 활용하여 Redis 사용하는 예제

## Acl Redis
* [Acl redis - One C++ redis client library in Acl](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md)
* ACL은 다양한 기능을 제공하는 C++ 라이브러리로, 네트워크 프로그래밍과 코루틴, Redis와 같은 데이터베이스와의 상호작용 등을 쉽게 할 수 있게 해줍니다.

## 코드 설명

#### 1. `RedisServerAcl.cpp`

```cpp

#include <iostream>
#include "pch.h"
#include "redisHandler.h"

int main() {
    acl::acl_cpp_init();  // ACL 초기화

    // Redis 서버의 주소와 포트 설정
    const char* redis_addr = "127.0.0.1:6379";
    int conn_timeout = 10;  // 연결 타임아웃 (초)
    int rw_timeout = 10;    // 읽기/쓰기 타임아웃 (초)

    // Redis 클라이언트 생성
    acl::redis_client client(redis_addr, conn_timeout, rw_timeout);
    acl::redis cmd(&client);  // Redis 명령 객체 생성

    const char* key = "my_key";
    const char* value = "my_value";
    int ttl = 300;  // 5분 (300초)

    // 데이터를 Redis에 5분 동안 저장
    set_with_ttl(cmd, key, value, ttl);

    // 저장된 데이터를 Redis에서 조회
    get_value(cmd, key);

    return 0;
}


```


- **`acl::acl_cpp_init()`**: ACL 라이브러리 초기화 함수입니다. 이 함수를 호출하여 ACL 내부의 여러 기능을 사용할 준비를 합니다.
- **`acl::redis_client client(redis_addr, conn_timeout, rw_timeout)`**: `redis_client` 객체를 생성하며, 이는 Redis 서버에 연결하기 위한 클라이언트입니다. `redis_addr`는 Redis 서버의 주소, `conn_timeout`은 연결 타임아웃, `rw_timeout`은 읽기/쓰기 타임아웃을 설정합니다.
- **`acl::redis cmd(&client)`**: Redis 명령어를 실행하는 객체입니다. `cmd` 객체를 통해 Redis에 명령을 보내고 응답을 받습니다.
- **`set_with_ttl(cmd, key, value, ttl)`**: 주어진 키와 값을 Redis에 저장하며, TTL(Time To Live)을 설정하여 해당 데이터가 TTL만큼의 시간이 지나면 자동으로 삭제됩니다.
- **`get_value(cmd, key)`**: Redis에 저장된 특정 키의 값을 조회합니다.


#### 2. `redisHandler.cpp`

```cpp
#include "redisHandler.h"
#include <iostream>

void set_with_ttl(acl::redis& cmd, const char* key, const char* value, int ttl) {
    if (cmd.setex(key, value, ttl) == false) {
        std::cerr << "Failed to set key with TTL in Redis" << std::endl;
    } else {
        std::cout << "Data saved with " << ttl << " seconds TTL." << std::endl;
    }
}

void get_value(acl::redis& cmd, const char* key) {
    acl::string value;
    if (cmd.get(key, value) == false) {
        std::cerr << "Failed to get value from Redis" << std::endl;
    } else {
        std::cout << "Value for key '" << key << "': " << value.c_str() << std::endl;
    }
}
```

- **`set_with_ttl()`**: 이 함수는 Redis에 데이터를 저장하는 역할을 합니다. `cmd.setex()`를 통해 특정 키(`key`)에 값을 저장하고, 해당 키가 지정된 TTL(예: 300초) 후에 자동으로 만료되도록 설정합니다.
  - `cmd.setex(key, value, ttl)`은 Redis의 `SETEX` 명령어와 동일하게 동작하며, TTL을 설정한 상태로 값을 저장합니다. 성공 여부에 따라 성공 또는 실패 메시지를 출력합니다.

- **`get_value()`**: 이 함수는 Redis에 저장된 키의 값을 조회합니다. `cmd.get()`을 통해 키에 해당하는 값을 가져오며, 값이 정상적으로 조회되면 값을 출력하고, 실패 시 오류 메시지를 출력합니다.


## ACL에서의 Redis 사용법

1. **Redis 클라이언트 초기화**:
   - ACL에서 Redis 서버에 연결하기 위해 `acl::redis_client` 객체를 생성합니다. 생성 시에 Redis 서버 주소, 연결 타임아웃, 읽기/쓰기 타임아웃을 설정할 수 있습니다.

2. **Redis 명령어 실행**:
   - `acl::redis` 객체를 통해 Redis 명령어를 실행합니다. 이 객체는 Redis 클라이언트와 연결되어 있으며, 다양한 Redis 명령어(`GET`, `SET`, `SETEX` 등)를 실행할 수 있는 메서드를 제공합니다.

3. **데이터 설정**:
   - `SETEX` 명령어를 통해 Redis에 데이터를 저장하면서 TTL을 설정할 수 있습니다. `setex()` 메서드는 이 역할을 하며, 성공 여부를 반환합니다.

4. **데이터 조회**:
   - `GET` 명령어를 사용하여 Redis에 저장된 특정 키의 값을 조회할 수 있습니다. `get()` 메서드는 조회된 데이터를 반환하며, 성공 여부에 따라 값을 출력하거나 실패 메시지를 출력할 수 있습니다.



-------------------

간단한 get, set 외에도 아래와 같은 다양한 기능을 사용할 수 있습니다. 

- [cluster example for redis3.0+](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#22-redis-client-cluster-example-for-redis30)
- [Using redis client cluster in multi-threads](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#23-using-redis-client-cluster-in-multi-threads)
- [redis pipeline in multi-threads](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md#24-use-redis-pipeline-in-multi-threads)


추후 추가 예정입니다.


