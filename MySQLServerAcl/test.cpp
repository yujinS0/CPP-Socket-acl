//#include "lib_acl.h"
//#include "acl_cpp/lib_acl.hpp"
//#include "pch.h" 
//
//// ������ test_tbl ���̺� ���� SQL ����
//const char* CREATE_TBL =
//"CREATE TABLE IF NOT EXISTS test_tbl\r\n"
//"(\r\n"
//"id INT AUTO_INCREMENT PRIMARY KEY,\r\n"
//"name VARCHAR(100) NOT NULL,\r\n"
//"age INT NOT NULL\r\n"
//")";
//
//// ���̺� ���� �Լ�
//static bool tbl_create(acl::db_handle& db)
//{
//    if (db.tbl_exists("test_tbl")) {
//        printf("Table 'test_tbl' already exists.\r\n");
//        return true;
//    }
//    if (db.sql_update(CREATE_TBL) == false) {
//        printf("SQL error while creating table.\r\n");
//        return false;
//    }
//    printf("Table 'test_tbl' created successfully.\r\n");
//    return true;
//}
//
//// ������ ���� �Լ�
////static bool tbl_insert(acl::db_handle& db, const char* name, int age)
////{
////    const char* sql_fmt = "INSERT INTO test_tbl(name, age) VALUES('%s', %d)";
////
////    acl::string sql;
////    sql.format(sql_fmt, name, age);  // ���� ����ؼ� ���� ����
////   
////
////    if (db.sql_update(sql.c_str()) == false) {
////        printf("Insert SQL error: %s\r\n", db.get_error());
////        return false;
////    }
////
////    printf("Data inserted successfully.\r\n");
////    db.free_result();
////    return true;
////}
//static bool tbl_insert(acl::db_handle& db, const char* name, int age)
//{
//    const char* sql_fmt = "INSERT INTO test_tbl(name, age) VALUES('%s', %d)";
//    acl::string sql;
//    sql.format(sql_fmt, name, age);  // ���� ����ؼ� ���� ����
//
//    // ���� ����� ������ ����
//    bool result = db.sql_update(sql.c_str());
//
//    // ���� ���� �� ���� �޽��� ���
//    if (!result) {
//        printf("Insert SQL error: %s\r\n", db.get_error());
//        return false;
//    }
//
//    printf("Data inserted successfully.\r\n");
//    db.free_result();
//    return true;
//}
//
//
//// ������ ���� �Լ�
//static bool tbl_select(acl::db_handle& db)
//{
//    const char* sql = "SELECT * FROM test_tbl";
//
//    bool isSelect = db.sql_select(sql);
//    if (isSelect == false) {
//        printf("Select SQL error: %s\r\n", db.get_error());
//        return false;
//    }
//
//    const acl::db_rows* result = db.get_result();
//
//    if (result == NULL) {
//        printf("No result found. Result pointer is NULL.\r\n");
//        db.free_result();
//        return false;
//    }
//
//    const std::vector<acl::db_row*>& rows = result->get_rows();
//    if (rows.empty()) {
//        printf("No data found.\r\n");
//    }
//
//    for (size_t i = 0; i < rows.size(); i++) {
//        const acl::db_row* row = rows[i];
//        printf("ID: %s, Name: %s, Age: %s\r\n",
//            row->field_value("id"), row->field_value("name"), row->field_value("age"));
//    }
//
//    db.free_result();
//    return true;
//}
//
////static bool tbl_select(acl::db_handle& db)
////{
////    const char* sql = "SELECT * FROM test_tbl";
////
////    if (db.sql_select(sql) == false) {
////        printf("Select SQL error: %s\r\n", db.get_error());
////        return false;
////    }
////
////    const acl::db_rows* result = db.get_result();
////
////    //if (result) {
////        const std::vector<acl::db_row*>& rows = result->get_rows();
////        if (rows.empty()) {
////            printf("No data found.\r\n");
////        }
////        for (size_t i = 0; i < rows.size(); i++) {
////            const acl::db_row* row = rows[i];
////            printf("ID: %s, Name: %s, Age: %s\r\n",
////                row->field_value("id"), row->field_value("name"), row->field_value("age"));
////        }
////    /*}
////    else {
////        printf("No result found.\r\n");
////    }*/
////    db.free_result();
////    return true;
////}
//
//// ������ ������Ʈ �Լ�
//static bool tbl_update(acl::db_handle& db, int id, const char* new_name, int new_age)
//{
//    const char* sql_fmt = "UPDATE test_tbl SET name='%s', age=%d WHERE id=%d";
//
//    acl::string sql;
//    sql.format(sql_fmt, new_name, new_age, id);
//
//    if (db.sql_update(sql.c_str()) == false) {
//        printf("Update SQL error: %s\r\n", db.get_error());
//        return false;
//    }
//
//    printf("Data updated successfully.\r\n");
//    db.free_result();
//    return true;
//}
//
//// ������ ���� �Լ�
//static bool tbl_delete(acl::db_handle& db, int id)
//{
//    const char* sql_fmt = "DELETE FROM test_tbl WHERE id=%d";
//
//    acl::string sql;
//    sql.format(sql_fmt, id);
//
//    if (db.sql_update(sql.c_str()) == false) {
//        printf("Delete SQL error: %s\r\n", db.get_error());
//        return false;
//    }
//
//    printf("Data deleted successfully.\r\n");
//    db.free_result();
//    return true;
//}
//
//// ���� �Լ�
//int main(void)
//{
//    acl::acl_cpp_init();
//    acl::log::stdout_open(true);
//
//    const char* libname = "libmysql.dll";
//    acl::string path;
//    path.format("C:\\Program Files\\MySQL\\MySQL Server 8.0\\lib\\%s", libname);
//    acl::db_handle::set_loadpath(path);
//
//    const char* dbaddr = "127.0.0.1:3306";
//    const char* dbname = "testdb";
//    const char* dbuser = "root";
//    const char* dbpass = "000930";
//
//    acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);
//
//    if (db.open() == false) {
//        printf("Failed to open database '%s'.\r\n", dbname);
//        return 1;
//    }
//
//    printf("Connected to database '%s'.\r\n", dbname);
//
//    // ���̺� ����
//    if (tbl_create(db) == false) {
//        return 1;
//    }
//
//    // ����� �Է��� ���� CRUD �۾� ����
//    char action;
//    char name[100];
//    int id, age;
//
//    while (true) {
//        printf("\nChoose an action: [i]nsert, [s]elect, [u]pdate, [d]elete, [q]uit: ");
//        scanf(" %c", &action);
//
//        switch (action) {
//        case 'i':  // ������ ����
//            printf("Enter name: ");
//            scanf("%s", name);
//            printf("Enter age: ");
//            scanf("%d", &age);
//            tbl_insert(db, name, age);
//            break;
//
//        case 's':  // ������ ��ȸ
//            printf("Select all data:\r\n");
//            tbl_select(db);
//            break;
//
//        case 'u':  // ������ ������Ʈ
//            printf("Enter ID to update: ");
//            scanf("%d", &id);
//            printf("Enter new name: ");
//            scanf("%s", name);
//            printf("Enter new age: ");
//            scanf("%d", &age);
//            tbl_update(db, id, name, age);
//            break;
//
//        case 'd':  // ������ ����
//            printf("Enter ID to delete: ");
//            scanf("%d", &id);
//            tbl_delete(db, id);
//            break;
//
//        case 'q':  // ����
//            printf("Exiting...\r\n");
//            return 0;
//
//        default:
//            printf("Invalid action. Try again.\r\n");
//            break;
//        }
//    }
//
//    return 0;
//}
