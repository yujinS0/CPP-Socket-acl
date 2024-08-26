//#include "mysql_functions.h"
//#include <print>
//
//// 간단한 test_tbl 테이블 생성 SQL 쿼리
//const char* CREATE_TBL =
//"CREATE TABLE IF NOT EXISTS test_tbl\r\n"
//"(\r\n"
//"id INT AUTO_INCREMENT PRIMARY KEY,\r\n"
//"name VARCHAR(100) NOT NULL,\r\n"
//"age INT NOT NULL\r\n"
//")";
//
//// Create
//bool tbl_create(acl::db_handle& db)
//{
//    if (db.tbl_exists("test_tbl")) {
//        std::print("Table 'test_tbl' already exists.\r\n");
//        return true;
//    }
//    if (db.sql_update(CREATE_TBL) == false) {
//        std::print("SQL error while creating table.\r\n");
//        return false;
//    }
//    std::print("Table 'test_tbl' created successfully.\r\n");
//    return true;
//}
//
//// Insert
//bool tbl_insert(acl::db_handle& db, const char* name, int age)
//{
//    acl::query query;
//
//    query.create_sql("INSERT INTO test_tbl(name, age) VALUES(:name, :age)")
//        .set_parameter("name", name)
//        .set_parameter("age", age);
//
//    if (db.exec_update(query) == false) {
//        std::print("Insert SQL error: %s\r\n", db.get_error());
//        return false;
//    }
//
//    std::print("Data inserted successfully.\r\n");
//    return true;
//}
//
//// Select
//bool tbl_select(acl::db_handle& db)
//{
//    acl::query query;
//    query.create_sql("select * from test_tbl")
//        .set_format("id", "id")
//        .set_format("name", "name")
//        .set_format("age", "age");
//
//    if (db.exec_select(query) == false)
//    {
//        std::print("select sql error\r\n");
//        return false;
//    }
//
//    const acl::db_rows* result = db.get_result();
//    if (result == NULL) {
//        std::print("No result found.\r\n");
//        return false;
//    }
//
//    const std::vector<acl::db_row*>& rows = result->get_rows();
//    if (rows.empty()) {
//        std::print("No data found.\r\n");
//        return false;
//    }
//
//    for (size_t i = 0; i < rows.size(); i++) {
//        const acl::db_row* row = rows[i];
//        std::print("ID: %s, Name: %s, Age: %s\r\n",
//            row->field_value("id"), row->field_value("name"), row->field_value("age"));
//    }
//
//    db.free_result();
//    return true;
//}
//
//// Update
//bool tbl_update(acl::db_handle& db, int id, const char* new_name, int new_age)
//{
//    acl::query query;
//
//    query.create_sql("UPDATE test_tbl SET name=:name, age=:age WHERE id=:id")
//        .set_parameter("name", new_name)
//        .set_parameter("age", new_age)
//        .set_parameter("id", id);
//
//    if (db.exec_update(query) == false) {
//        std::print("Update SQL error: %s\r\n", db.get_error());
//        return false;
//    }
//
//    std::print("Data updated successfully.\r\n");
//    return true;
//}
//
//// Delete
//bool tbl_delete(acl::db_handle& db, int id)
//{
//    acl::query query;
//
//    query.create_sql("DELETE FROM test_tbl WHERE id=:id")
//        .set_parameter("id", id);
//
//    if (db.exec_update(query) == false) {
//        std::print("Delete SQL error: %s\r\n", db.get_error());
//        return false;
//    }
//
//    std::print("Data deleted successfully.\r\n");
//    return true;
//}