#include "pch.h"
#include "tcpServer.h"
#include "tcpCoroutine.h"
#include "redisHandler.h"

int main() {
    //std::cout << "__cplusplus value: " << __cplusplus << std::endl; // C++ ���� Ȯ�ο�
    acl::acl_cpp_init();  // ACL �ʱ�ȭ

    //run_tcp_server();  // TCP ���� ���� �Լ� ȣ��
    //run_tcp_coroutine_server();  // �ڷ�ƾ TCP ���� ���� �Լ� ȣ��

    // Redis ������ �ּҿ� ��Ʈ ����
    const char* redis_addr = "127.0.0.1:6379";
    int conn_timeout = 10;  // ���� Ÿ�Ӿƿ� (��)
    int rw_timeout = 10;    // �б�/���� Ÿ�Ӿƿ� (��)

    // Redis Ŭ���̾�Ʈ ����
    acl::redis_client client(redis_addr, conn_timeout, rw_timeout);
    acl::redis cmd(&client);  // Redis ��� ��ü ����

    const char* key = "my_key";
    const char* value = "my_value";
    int ttl = 300;  // 5�� (300��)

    // �����͸� Redis�� 5�� ���� ����
    set_with_ttl(cmd, key, value, ttl);

    // ����� �����͸� Redis���� ��ȸ
    get_value(cmd, key);


    return 0;
}
