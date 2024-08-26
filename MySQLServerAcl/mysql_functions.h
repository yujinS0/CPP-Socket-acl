#pragma once
#include "lib_acl.h"
#include "acl_cpp/lib_acl.hpp"


// ������ test_tbl ���̺� ���� SQL ����
extern const char* CREATE_TBL;

// ���̺� ���� �Լ�
bool tbl_create(acl::db_handle& db);

// ������ ���� �Լ�
bool tbl_insert(acl::db_handle& db, const char* name, int age);

// ������ ���� �Լ�
bool tbl_select(acl::db_handle& db);

// ������ ������Ʈ �Լ�
bool tbl_update(acl::db_handle& db, int id, const char* new_name, int new_age);

// ������ ���� �Լ�
bool tbl_delete(acl::db_handle& db, int id);
