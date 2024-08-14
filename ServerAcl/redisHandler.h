#pragma once
#ifndef REDIS_HANDLER_H
#define REDIS_HANDLER_H

#include "acl_cpp/lib_acl.hpp"

// �����͸� Redis�� 5�� ���� �����ϴ� �Լ�
void set_with_ttl(acl::redis& cmd, const char* key, const char* value, int ttl);

// Redis���� �����͸� ��ȸ�ϴ� �Լ�
void get_value(acl::redis& cmd, const char* key);

#endif // REDIS_HANDLER_H
