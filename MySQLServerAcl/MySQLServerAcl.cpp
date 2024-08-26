//#include <iostream>
//#include <cstdlib>
//#include <stdio.h>
//#include <stdlib.h>
//#include "pch.h"
//#include <mysql.h>
//#include "lib_acl.h"
//#include "acl_cpp/lib_acl.hpp"
//
//// 테이블 생성 SQL
//const char* CREATE_TBL =
//"CREATE TABLE IF NOT EXISTS test_tbl ("
//"id INT AUTO_INCREMENT PRIMARY KEY, "
//"name VARCHAR(100) NOT NULL, "
//"age INT NOT NULL)";
//
//// 데이터 삽입 SQL
//const char* INSERT_SQL =
//"INSERT INTO test_tbl (name, age) VALUES ('John Doe', 30)";
//
//// 데이터 조회 SQL
//const char* SELECT_SQL =
//"SELECT * FROM test_tbl";
//
//// 데이터 삭제 SQL
//const char* DELETE_SQL =
//"DELETE FROM test_tbl WHERE name = 'John Doe'";
//
//bool create_table(acl::db_handle& db) {
//    printf("Executing create table query...\n");
//
//    if (db.tbl_exists("1111")) {
//        printf("table exist\r\n");
//        return true;
//    }
//    else if (db.sql_update(CREATE_TBL) == false) {
//        printf("sql error\r\n");
//        return false;
//    }
//    else {
//        printf("create table ok\r\n");
//        return true;
//    }
//    //// 쿼리 문자열 출력
//    //printf("Query: %s\n", CREATE_TBL);
//
//    //if (!db.sql_update(CREATE_TBL)) {
//    //    // 쿼리 실행에 실패한 경우 오류 메시지를 출력
//    //    const char* error = db.get_error();
//    //    if (error != nullptr) {
//    //        printf("Error creating table: %s\n", error);
//    //    }
//    //    else {
//    //        printf("Error creating table: Unknown error (db.get_error() returned null)\n");
//    //    }
//    //    return false;
//    //}
//
//    printf("Table created successfully\n");
//    return true;
//}
//
//
//
//// 데이터 삽입 함수
//bool insert_data(acl::db_handle& db) {
//    if (!db.sql_update(INSERT_SQL)) {
//        printf("Error inserting data: %s\n", db.get_error());
//        return false;
//    }
//    printf("Data inserted successfully\n");
//    return true;
//}
//
//// 데이터 조회 함수
//bool select_data(acl::db_handle& db) {
//    if (!db.sql_select(SELECT_SQL)) {
//        printf("Error selecting data: %s\n", db.get_error());
//        return false;
//    }
//
//    const acl::db_rows* result = db.get_result();
//    if (result) {
//        const std::vector<acl::db_row*>& rows = result->get_rows();
//        for (size_t i = 0; i < rows.size(); ++i) {
//            const acl::db_row* row = rows[i];
//            printf("ID: %s, Name: %s, Age: %s\n",
//                (*row)["id"], (*row)["name"], (*row)["age"]);
//        }
//    }
//
//    db.free_result();
//    return true;
//}
//
//// 데이터 삭제 함수
//bool delete_data(acl::db_handle& db) {
//    if (!db.sql_update(DELETE_SQL)) {
//        printf("Error deleting data: %s\n", db.get_error());
//        return false;
//    }
//    printf("Data deleted successfully\n");
//    return true;
//}
//
//int main(void) {
//    // ACL 라이브러리 초기화
//    acl::acl_cpp_init();
//
//    // 표준 출력 로그 활성화
//    acl::log::stdout_open(true);
//
//    // 데이터베이스 연결 정보 설정
//    const char* dbaddr = "127.0.0.1:3306";
//    const char* dbname = "testdb";
//    const char* dbuser = "root";
//    const char* dbpass = "000930";
//
//    // 데이터베이스 객체 생성
//    acl::db_mysql mysql_db(dbaddr, dbname, dbuser, dbpass);
//    acl::db_handle& db = mysql_db;  // db_handle을 통해 사용
//
//    // 데이터베이스 연결
//    if (!db.open()) {
//        printf("Error opening database: %s\n", db.get_error());
//        return 1;
//    }
//    printf("Database connected successfully\n");
//
//    // 테이블 생성
//    if (!create_table(db)) {
//        return 1;
//    }
//
//    // 데이터 삽입
//    if (!insert_data(db)) {
//        return 1;
//    }
//
//    // 데이터 조회
//    if (!select_data(db)) {
//        return 1;
//    }
//
//    // 데이터 삭제
//    if (!delete_data(db)) {
//        return 1;
//    }
//
//    // 데이터베이스 연결 종료
//    db.close();
//
//    printf("Test completed successfully\n");
//    return 0;
//}
//
//
////// 레코드 생성 (INSERT)
////static int tbl_insert(acl::db_handle& db, const char* value, const char* category, const char* type) {
////    // INSERT SQL문 작성
////    acl::string sql;
////    sql.format("INSERT INTO black_white_list (value, category, type) VALUES ('%s', '%s', '%s')", value, category, type);
////
////    // SQL 실행
////    if (db.sql_update(sql) == false) {
////        printf("INSERT sql error\r\n");
////        return (-1);
////    }
////
////    printf("Record inserted successfully.\r\n");
////    return 0;
////}
////
////// 레코드 조회 (SELECT)
////static int tbl_select(acl::db_handle& db) {
////    const char* sql = "SELECT value, category, type FROM black_white_list";
////
////    if (db.sql_select(sql) == false) {
////        printf("SELECT sql error\r\n");
////        return (-1);
////    }
////
////    printf("\r\n---------------------------------------------------\r\n");
////
////    const acl::db_rows* result = db.get_result();
////    if (result) {
////        const std::vector<acl::db_row*>& rows = result->get_rows();
////        for (size_t i = 0; i < rows.size(); i++) {
////            const acl::db_row* row = rows[i];
////            for (size_t j = 0; j < row->length(); j++) {
////                printf("%s, ", (*row)[j]);
////            }
////            printf("\r\n");
////        }
////    }
////
////    int ret = (int)db.length();
////    db.free_result();
////    return ret;
////}
////
////// 레코드 업데이트 (UPDATE)
////static int tbl_update(acl::db_handle& db, const char* value, const char* new_category, const char* new_type) {
////    // UPDATE SQL문 작성
////    acl::string sql;
////    sql.format("UPDATE black_white_list SET category = '%s', type = '%s' WHERE value = '%s'", new_category, new_type, value);
////
////    // SQL 실행
////    if (db.sql_update(sql) == false) {
////        printf("UPDATE sql error\r\n");
////        return (-1);
////    }
////
////    printf("Record updated successfully.\r\n");
////    return 0;
////}
////
////// 레코드 삭제 (DELETE)
////static int tbl_delete(acl::db_handle& db, const char* value) {
////    // DELETE SQL문 작성
////    acl::string sql;
////    sql.format("DELETE FROM black_white_list WHERE value = '%s'", value);
////
////    // SQL 실행
////    if (db.sql_update(sql) == false) {
////        printf("DELETE sql error\r\n");
////        return (-1);
////    }
////
////    printf("Record deleted successfully.\r\n");
////
////    return 0;
////}
////
////int main() {
////    SetConsoleOutputCP(CP_UTF8);
////
////    // ACL 및 로그 초기화
////    acl::acl_cpp_init();
////    acl::log::stdout_open(true);
////
////    const char* libname = "libmysql.dll";
////    
////    // MySQL 라이브러리 로드 경로 설정
////    acl::string path;
////    path.format("C:\\Program Files\\MySQL\\MySQL Server 8.0\\lib\\libmysql.dll");
////
////    printf("%s path: %s\r\n", libname, path.c_str());
////
////    // MySQL 라이브러리 경로 설정
////    acl::db_handle::set_loadpath(path);
////
////    // MySQL 서버 연결 정보
////    const char* dbaddr = "127.0.0.1:3306";  // MySQL 서버 주소
////    const char* dbuser = "root";         // MySQL 사용자 이름
////    const char* dbpass = "000930"; // MySQL 사용자 비밀번호
////    const char* dbname = "testdb";   // 연결할 데이터베이스 이름
////
////    // MySQL 데이터베이스 연결
////    //acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);
////    acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);
////
////    // MySQL 서버 연결 시도
////    /*if (db.open() == false) {
////        std::cerr << "데이터베이스 연결 실패: " << dbname << std::endl;
////        std::cerr << "Error: " << std::endl;
////        return 1;
////    }*/
////
////    //////////////////////////////////////////////////
////    // 데이터베이스 열기 (dbopen을 먼저 호출)
////    if (db.dbopen() == false) {
////        printf("Failed to open db: %s\r\n", dbname);
////        getchar();
////        return 1;
////    }
////
////    //// SSL 비활성화 설정
////    //MYSQL* mysql_conn = (MYSQL*)db.get_conn();
////    //if (mysql_conn == NULL) {
////    //    printf("Failed to get MySQL connection object.\r\n");
////    //    return 1;
////    //}
////
////    //// SSL 비활성화
////    //mysql_options(mysql_conn, MYSQL_OPT_SSL_MODE, (void*)SSL_MODE_DISABLED);
////
////    //printf("Connected to database: %s\r\n", dbname);
////
////    
////    //acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);
////    //if (db.dbopen(NULL) == false) {
////    //    printf("Failed to open db: %s\r\n", dbname);
////    //    getchar();
////    //    return 1;
////    //}
////
////    //// SSL 비활성화 설정
////    //MYSQL* mysql_conn = (MYSQL*)db.get_conn();
////    //mysql_options(mysql_conn, MYSQL_OPT_SSL_MODE, (void*)SSL_MODE_DISABLED);
////
////    // CRUD 작업 실행
////    char choice;
////    while (true) {
////        printf("\nChoose an action: [i]nsert, [s]elect, [u]pdate, [d]elete, [q]uit: ");
////        std::cin >> choice;
////
////        if (choice == 'i') {
////            // 데이터 삽입 예시
////            tbl_insert(db, "example_value", "example_category", "example_type");
////        }
////        else if (choice == 's') {
////            // 데이터 조회
////            tbl_select(db);
////        }
////        else if (choice == 'u') {
////            // 데이터 업데이트
////            tbl_update(db, "example_value", "new_category", "new_type");
////        }
////        else if (choice == 'd') {
////            // 데이터 삭제
////            tbl_delete(db, "example_value");
////        }
////        else if (choice == 'q') {
////            break;
////        }
////        else {
////            printf("Invalid option. Please try again.\r\n");
////        }
////    }
////
////    // MySQL 클라이언트 정보 출력
////    printf("MySQL client version: %ld, info: %s\r\n",
////        db.mysql_libversion(), db.mysql_client_info());
////
////    return 0;
////}
