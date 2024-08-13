#include "pch.h"
#include "tcpServer.h"
#include "tcpCoroutine.h"

int main() {
    try{
        //std::cout << "__cplusplus value: " << __cplusplus << std::endl; // C++ 버전 확인용
        acl::acl_cpp_init();  // ACL 초기화
        //run_tcp_server();  // TCP 서버 실행 함수 호출
        run_tcp_coroutine_server();  // 코루틴 TCP 서버 실행 함수 호출
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    return 0;
}
