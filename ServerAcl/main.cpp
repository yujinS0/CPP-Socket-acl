#include "pch.h"
#include "tcpServer.h"

int main() {
    acl::acl_cpp_init();  // ACL �ʱ�ȭ
    run_tcp_server();  // TCP ���� ���� �Լ� ȣ��
    return 0;
}