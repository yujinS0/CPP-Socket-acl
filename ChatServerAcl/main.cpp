#include "pch.h"
#include "tcpCoroutine.h"
#include "redisHandler.h"

int main() {
    //std::cout << "__cplusplus value: " << __cplusplus << std::endl; // C++ ���� Ȯ�ο�
    acl::acl_cpp_init();  // ACL �ʱ�ȭ

    run_tcp_coroutine_server_with_redis_and_rooms();  // �ڷ�ƾ TCP ���� ���� �Լ� ȣ��

    return 0;
}
