﻿#pragma once
#ifndef REDIS_HANDLER_H
#define REDIS_HANDLER_H

#include <string>
#include <iostream>
#include "acl_cpp/lib_acl.hpp"
#include "User.h"

// Redis 연결 설정
acl::redis_client* setup_redis();
acl::redis* setup_redis(const char* redis_addr);

// Redis 명령어 처리 함수
void handle_string(acl::redis& redis);
void handle_get(acl::redis& redis);
void handle_list_push(acl::redis_list& redis);
void handle_list_get(acl::redis_list& redis);
void handle_set(acl::redis& redis);
void handle_hash(acl::redis& redis);
void handle_zset(acl::redis& redis);

// JSON 관련 핸들러 함수
void handle_set_json(acl::redis_client& client);
void handle_get_json(acl::redis_client& client);

bool set_json_data(acl::redis_client& client, const std::string& key, const User& user);
std::string get_json_field(acl::redis_client& client, const std::string& key, const std::string& field);


// 이전 기본 get/set 함수
void set_with_ttl(acl::redis& cmd, const char* key, const char* value, int ttl);
void get_value(acl::redis& cmd, const char* key);

#endif // REDIS_HANDLER_H
