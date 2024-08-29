#pragma once
#include "acl_cpp/lib_acl.hpp"
#include <string_view>

// ������ test_tbl ���̺� ���� SQL ����
extern const char* CREATE_TBL;

// ���̺� ���� �Լ�
bool tbl_create(acl::db_handle& db);

// ������ ���� �Լ�
bool tbl_insert(acl::db_handle& db, std::string_view name, int age);

// ������ ���� �Լ�
[[nodiscard]] bool tbl_select(acl::db_handle& db);

// ������ ������Ʈ �Լ�
bool tbl_update(acl::db_handle& db, int id, std::string_view new_name, int new_age);

// ������ ���� �Լ�
bool tbl_delete(acl::db_handle& db, int id);
