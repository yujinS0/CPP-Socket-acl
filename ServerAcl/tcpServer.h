#pragma once
#include "acl_cpp/lib_acl.hpp"

void run_tcp_server();  // TCP ���� ���� �Լ� ����
void handle_client(acl::socket_stream* conn);  // Ŭ���̾�Ʈ ó�� �Լ� ����