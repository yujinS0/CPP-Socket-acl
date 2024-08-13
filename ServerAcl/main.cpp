#include "pch.h"
#include "tcpServer.h"
#include "tcpCoroutine.h"

int main() {
    try{
        //std::cout << "__cplusplus value: " << __cplusplus << std::endl; // C++ ���� Ȯ�ο�
        acl::acl_cpp_init();  // ACL �ʱ�ȭ
        //run_tcp_server();  // TCP ���� ���� �Լ� ȣ��
        run_tcp_coroutine_server();  // �ڷ�ƾ TCP ���� ���� �Լ� ȣ��
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    return 0;
}
