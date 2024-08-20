#include "pch.h"

// MySQL 관련 설정 및 초기화
const char* DB_HOST = "127.0.0.1:3306";
const char* DB_NAME = "testdb";
const char* DB_USER = "root";
const char* DB_PASS = "";

class mysql_fiber : public acl::fiber
{
public:
    mysql_fiber(acl::db_pool& dbpool) : dbpool_(dbpool) {}
    ~mysql_fiber() {}

protected:
    void run(void) override
    {
        acl::db_handle* db = dbpool_.peek_open();
        if (!db) {
            std::cerr << "DB Connection Failed!" << std::endl;
            return;
        }

        // Create Table
        const char* create_table_sql = "CREATE TABLE IF NOT EXISTS users ("
            "id INT AUTO_INCREMENT PRIMARY KEY, "
            "name VARCHAR(100), "
            "age INT);";
        db->sql_update(create_table_sql);

        // Insert Data
        const char* insert_sql = "INSERT INTO users (name, age) VALUES ('John Doe', 30);";
        db->sql_update(insert_sql);

        // Select Data
        const char* select_sql = "SELECT * FROM users;";
        if (db->sql_select(select_sql)) {
            const acl::db_rows* result = db->get_result();
            const auto& rows = result->get_rows();
            for (size_t i = 0; i < rows.size(); ++i) {
                const acl::db_row* row = rows[i];
                std::cout << "ID: " << (*row)["id"] << ", Name: " << (*row)["name"] << ", Age: " << (*row)["age"] << std::endl;
            }
            db->free_result();
        }

        // Update Data
        const char* update_sql = "UPDATE users SET age = 31 WHERE name = 'John Doe';";
        db->sql_update(update_sql);

        // Delete Data
        const char* delete_sql = "DELETE FROM users WHERE name = 'John Doe';";
        db->sql_update(delete_sql);

        dbpool_.put(db);

        // 코루틴 종료
        acl::fiber::schedule_stop();
    }

private:
    acl::db_pool& dbpool_;
};

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    // 라이브러리 초기화
    acl::acl_cpp_init();
    acl::log::stdout_open(true);

    // MySQL 연결 설정
    acl::mysql_conf dbconf(DB_HOST, DB_NAME);
    dbconf.set_dbuser(DB_USER).set_dbpass(DB_PASS).set_dblimit(2).set_charset("utf8");

    // MySQL 연결 풀 생성
    acl::mysql_pool dbpool(dbconf);

    // 코루틴 시작
    acl::fiber* f = new mysql_fiber(dbpool);
    f->start();

    // 스케줄러 실행
    acl::fiber::schedule();

    return 0;
}
