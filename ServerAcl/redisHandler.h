#pragma once
#ifndef REDIS_HANDLER_H
#define REDIS_HANDLER_H

#include "acl_cpp/lib_acl.hpp"

// 데이터를 Redis에 5분 동안 저장하는 함수
void set_with_ttl(acl::redis& cmd, const char* key, const char* value, int ttl);

// Redis에서 데이터를 조회하는 함수
void get_value(acl::redis& cmd, const char* key);

#endif // REDIS_HANDLER_H
