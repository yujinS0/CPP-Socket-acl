//#include <iostream>
//#include <mysql.h>  // MySQL C API ��� ����
//#include <cstdlib>
//#include <stdio.h>
//#include <stdlib.h>
//#include "pch.h"
//
//// MySQL C API ��� ������ ���� mysql �ǽ� �����ϴ� ���� (acl �̿� X)
//
//
//void finish_with_error(MYSQL* conn) {
//    std::cerr << "MySQL error: " << mysql_error(conn) << std::endl;
//    mysql_close(conn);  // MySQL ���� ����
//    exit(EXIT_FAILURE); // ���α׷� ����
//}
//
//
//int main_mysql_cAPI() {
//    SetConsoleOutputCP(CP_UTF8);
//    // MySQL �ڵ鷯 ����
//    MYSQL* conn;
//
//    // MySQL �ڵ鷯 �ʱ�ȭ
//    conn = mysql_init(NULL);
//
//    // MySQL ���� ���� ����
//    const char* server = "127.0.0.1";  // MySQL ���� �ּ�
//    const char* user = "root";         // MySQL ����� �̸�
//    const char* password = "000930"; // MySQL ����� ��й�ȣ
//    const char* database = "testdb";   // ������ �����ͺ��̽� �̸�
//    unsigned int port = 3306;          // MySQL ��Ʈ ��ȣ
//
//    // MySQL ������ ����
//    if (conn == NULL) {
//        std::cerr << "mysql_init() failed\n";
//        return EXIT_FAILURE;
//    }
//
//    if (mysql_real_connect(conn, server, user, password, database, port, NULL, 0) == NULL) {
//        std::cerr << "mysql_real_connect() failed\n";
//        std::cerr << "Error: " << mysql_error(conn) << std::endl;
//        mysql_close(conn);
//        return EXIT_FAILURE;
//    }
//
//    std::cout << "Successfully connected to MySQL server!" << std::endl;
//
//    // ���̺� ���� (���� �������� �ʴ´ٸ�)
//    if (mysql_query(conn, "CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255), age INT)")) {
//        finish_with_error(conn);
//    }
//
//    // ������ ����
//    if (mysql_query(conn, "INSERT INTO users (name, age) VALUES('John Doe', 30)")) {
//        finish_with_error(conn);
//    }
//
//    // ������ ��ȸ
//    if (mysql_query(conn, "SELECT * FROM users")) {
//        finish_with_error(conn);
//    }
//
//    MYSQL_RES* result = mysql_store_result(conn);
//    if (result == NULL) {
//        finish_with_error(conn);
//    }
//
//    int num_fields = mysql_num_fields(result);
//    MYSQL_ROW row;
//
//    std::cout << "ID | Name      | Age\n";
//    std::cout << "----------------------\n";
//
//    while ((row = mysql_fetch_row(result))) {
//        for (int i = 0; i < num_fields; i++) {
//            std::cout << (row[i] ? row[i] : "NULL") << " | ";
//        }
//        std::cout << std::endl;
//    }
//
//    mysql_free_result(result);
//
//    // ������ ������Ʈ
//    if (mysql_query(conn, "UPDATE users SET age = 35 WHERE name = 'John Doe'")) {
//        finish_with_error(conn);
//    }
//
//    // ������ ����
//    /*if (mysql_query(conn, "DELETE FROM users WHERE name = 'John Doe'")) {
//        finish_with_error(conn);
//    }*/
//
//    // MySQL ���� ����
//    mysql_close(conn);
//    std::cout << "CRUD �۾��� �Ϸ�Ǿ����ϴ�." << std::endl;
//
//    return EXIT_SUCCESS;
//}
//
////
////// MySQL ���� ���� �� �ʱ�ȭ
////const char* DB_HOST = "127.0.0.1:3306";
////const char* DB_NAME = "testdb";
////const char* DB_USER = "root";
////const char* DB_PASS = "000930";
////
//////int main() {
//////    MYSQL* conn;
//////    MYSQL_RES* res;
//////    MYSQL_ROW row;
//////
//////    const char* server = "localhost";
//////    const char* user = "root";
//////    const char* password = "000930";  // MySQL root ���� ��й�ȣ
//////    const char* database = "testdb";  // ����� �����ͺ��̽� �̸�
//////
//////    conn = mysql_init(NULL);
//////
//////    // MySQL ����
//////    if (!mysql_real_connect(conn, server, user, password, database, 3306, NULL, 0)) {
//////        std::cerr << "MySQL ���� ����: " << mysql_error(conn) << std::endl;
//////        return EXIT_FAILURE;
//////    }
//////
//////    // ������ ����
//////    if (mysql_query(conn, "INSERT INTO test_table (name, age) VALUES('John', 25)")) {
//////        std::cerr << "INSERT ����: " << mysql_error(conn) << std::endl;
//////        return EXIT_FAILURE;
//////    }
//////
//////    // ������ ��ȸ
//////    if (mysql_query(conn, "SELECT * FROM test_table")) {
//////        std::cerr << "SELECT ����: " << mysql_error(conn) << std::endl;
//////        return EXIT_FAILURE;
//////    }
//////
//////    res = mysql_store_result(conn);
//////    int num_fields = mysql_num_fields(res);
//////
//////    while ((row = mysql_fetch_row(res))) {
//////        for (int i = 0; i < num_fields; i++) {
//////            std::cout << (row[i] ? row[i] : "NULL") << " ";
//////        }
//////        std::cout << std::endl;
//////    }
//////
//////    // ���ҽ� ����
//////    mysql_free_result(res);
//////    mysql_close(conn);
//////
//////    return EXIT_SUCCESS;
//////}
////
////
////class mysql_fiber : public acl::fiber
////{
////public:
////    mysql_fiber(acl::db_pool& dbpool) : dbpool_(dbpool) {}
////    ~mysql_fiber() {}
////
////protected:
////    void run(void) override
////    {
////        acl::db_handle* db = dbpool_.peek_open();
////        if (!db) {
////            std::cerr << "DB Connection Failed!" << std::endl;
////            return;
////        }
////
////        // Create Table
////        const char* create_table_sql = "CREATE TABLE IF NOT EXISTS users ("
////            "id INT AUTO_INCREMENT PRIMARY KEY, "
////            "name VARCHAR(100), "
////            "age INT);";
////        db->sql_update(create_table_sql);
////
////        // Insert Data
////        const char* insert_sql = "INSERT INTO users (name, age) VALUES ('John Doe', 30);";
////        db->sql_update(insert_sql);
////
////        // Select Data
////        const char* select_sql = "SELECT * FROM users;";
////        if (db->sql_select(select_sql)) {
////            const acl::db_rows* result = db->get_result();
////            const auto& rows = result->get_rows();
////            for (size_t i = 0; i < rows.size(); ++i) {
////                const acl::db_row* row = rows[i];
////                std::cout << "ID: " << (*row)["id"] << ", Name: " << (*row)["name"] << ", Age: " << (*row)["age"] << std::endl;
////            }
////            db->free_result();
////        }
////
////        // Update Data
////        const char* update_sql = "UPDATE users SET age = 31 WHERE name = 'John Doe';";
////        db->sql_update(update_sql);
////
////        // Delete Data
////        const char* delete_sql = "DELETE FROM users WHERE name = 'John Doe';";
////        db->sql_update(delete_sql);
////
////        dbpool_.put(db);
////
////        // �ڷ�ƾ ����
////        acl::fiber::schedule_stop();
////    }
////
////private:
////    acl::db_pool& dbpool_;
////};
////
////int main(int argc, char* argv[])
////{
////    SetConsoleOutputCP(CP_UTF8);
////
////    // ���̺귯�� �ʱ�ȭ
////    acl::acl_cpp_init();
////    acl::log::stdout_open(true);
////
////    // MySQL ���� ����
////    acl::mysql_conf dbconf(DB_HOST, DB_NAME);
////    dbconf.set_dbuser(DB_USER).set_dbpass(DB_PASS).set_dblimit(2).set_charset("utf8");
////
////    // MySQL ���� Ǯ ����
////    acl::mysql_pool dbpool(dbconf);
////
////    // �ڷ�ƾ ����
////    acl::fiber* f = new mysql_fiber(dbpool);
////    f->start();
////
////    // �����ٷ� ����
////    acl::fiber::schedule();
////
////    return 0;
////}
