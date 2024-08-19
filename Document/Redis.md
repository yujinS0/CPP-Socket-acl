# Acl redis
Acl의 redis 모듈은 고성능, 풍부한 인터페이스, 사용하기 쉬운 강력한 redis 클라이언트 라이브러리입니다. <br>
Acl redis는 13개 이상의 C++ 클래스와 150개 이상의 명령어를 지원합니다. 
* STRING, HASH, LIST, SET, ZSET, HyperLogLog, PUBSUB, STREAM, TRANSACTION, SCRIPT, CONNECTION, SERVER <br>
Acl redis를 사용하는 사용자는 C++ STL 표준 인터페이스를 사용하는 것처럼 네트워크 통신, redis 프로토콜, 해시 슬롯 캐싱 등에 대해 신경 쓸 필요가 없습니다.

<br><br>
ACL (Advanced C/C++ Library)에서 제공하는 Redis 클라이언트를 사용하는 방법을 중점적으로 다루고 있습니다.

### 1. ACL Redis 클라이언트 개요

ACL 라이브러리의 `lib_acl_cpp` 모듈은 Redis와 같은 서버와 상호 작용하기 위한 클라이언트를 제공합니다.  <br>
이를 통해 Redis 서버와 연결하고 데이터를 저장하거나 조회할 수 있습니다. <br>
Acl redis는 lib_acl_cpp의 일부이므로 사용자는 Acl 프로젝트만 빌드하면 됩니다. <br>

### 2. 기본 설정 및 초기화

먼저, ACL Redis 클라이언트를 사용하려면 라이브러리 초기화가 필요합니다. 이를 위해 `acl_cpp_init()` 함수를 호출해야 합니다.

```cpp
#include "acl_cpp/lib_acl.hpp"

int main() {
    acl::acl_cpp_init();  // ACL 라이브러리 초기화

    // Redis 관련 코드들...
}
```

### 3. Redis 서버 연결 설정

Redis 클라이언트 인스턴스를 생성하고, Redis 서버에 연결해야 합니다.

```cpp
const char* redis_addr = "127.0.0.1:6379";  // Redis 서버 주소
acl::redis_client client(redis_addr, 0, 0);  // Redis 클라이언트 객체 생성

// 연결 시간 및 읽기/쓰기 타임아웃 설정
client.set_timeout(10, 10);  // 연결 타임아웃, 읽기/쓰기 타임아웃 설정 (초 단위)
```

- `redis_addr`: Redis 서버의 주소와 포트입니다.
- `redis_client`: 이 객체를 통해 Redis와 상호작용합니다.
- `set_timeout`: Redis 서버와의 연결에 대한 타임아웃을 설정합니다. (연결, 읽기, 쓰기)

### 4. 데이터 읽기/쓰기

ACL Redis 클라이언트를 통해 Redis 서버에 데이터를 쓰고, 읽는 방법을 보여드리겠습니다.

#### 4.1. 데이터 쓰기

다음은 키-값 쌍을 Redis에 저장하는 예제입니다:

```cpp
acl::redis cmd(&client);  // Redis 명령 객체 생성
bool result = cmd.set("my_key", "my_value");  // "my_key"라는 키에 "my_value" 저장

if (!result) {
    std::cerr << "Failed to set key-value in Redis" << std::endl;
}
```

- `set`: Redis의 `SET` 명령어와 동일하게, 특정 키에 값을 저장합니다.

#### 4.2. 데이터 읽기

다음은 Redis에 저장된 값을 읽어오는 예제입니다:

```cpp
acl::string value;
bool result = cmd.get("my_key", value);  // "my_key"에 해당하는 값을 가져옴

if (result) {
    std::cout << "Value of 'my_key': " << value.c_str() << std::endl;
} else {
    std::cerr << "Failed to get value from Redis" << std::endl;
}
```

- `get`: Redis의 `GET` 명령어와 동일하게, 특정 키에 저장된 값을 가져옵니다.

### 5. 예외 처리 및 오류 확인

ACL Redis 클라이언트는 명령 실행 시 오류가 발생하면 `false`를 반환합니다. 오류를 처리하기 위해 아래와 같은 방법으로 결과를 확인할 수 있습니다:

```cpp
if (!cmd.set("my_key", "my_value")) {
    std::cerr << "Error: " << cmd.result_error() << std::endl;
}
```

- `result_error`: 마지막 명령어 실행 시 발생한 오류 메시지를 반환합니다.

### 6. 클러스터 모드 사용 (선택 사항)

Redis 클러스터 모드를 사용할 경우, `redis_client_cluster` 객체를 사용하여 여러 Redis 노드에 연결할 수 있습니다.

```cpp
acl::redis_client_cluster cluster_client;
cluster_client.set("127.0.0.1:7000", 5);  // 여러 Redis 노드 추가

acl::redis cmd(&cluster_client);
```

- `set`: 클러스터 모드에서 사용할 노드와 연결을 설정합니다.

### 7. 종료 처리

작업이 완료된 후에는 사용한 Redis 클라이언트의 연결을 닫아야 합니다. 이는 `redis_client` 객체가 소멸될 때 자동으로 처리됩니다.


