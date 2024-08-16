#include <iostream>

#include "pch.h"
#include "redisHandler.h"

int main() {
    //std::cout << "__cplusplus value: " << __cplusplus << std::endl; // C++ 버전 확인용
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
