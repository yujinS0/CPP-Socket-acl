#pragma once
#include "lib_acl.h"
#include "acl_cpp/lib_acl.hpp"


// 간단한 test_tbl 테이블 생성 SQL 쿼리
extern const char* CREATE_TBL;

// 테이블 생성 함수
bool tbl_create(acl::db_handle& db);

// 데이터 삽입 함수
bool tbl_insert(acl::db_handle& db, const char* name, int age);

// 데이터 선택 함수
bool tbl_select(acl::db_handle& db);

// 데이터 업데이트 함수
bool tbl_update(acl::db_handle& db, int id, const char* new_name, int new_age);

// 데이터 삭제 함수
bool tbl_delete(acl::db_handle& db, int id);
