#include "pch.h"
#include "tcpCoroutine.h"
#include "redisHandler.h"

int main() {
    //std::cout << "__cplusplus value: " << __cplusplus << std::endl; // C++ 버전 확인용
    acl::acl_cpp_init();  // ACL 초기화

    run_tcp_coroutine_server_with_redis_and_rooms();  // 코루틴 TCP 서버 실행 함수 호출

    return 0;
}
