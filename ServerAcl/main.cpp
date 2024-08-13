#include "pch.h"
#include "tcpServer.h"

int main() {
    acl::acl_cpp_init();  // ACL 초기화
    run_tcp_server();  // TCP 서버 실행 함수 호출
    return 0;
}