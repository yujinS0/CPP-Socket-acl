# MySQL with Acl

이 문서는 `lib_acl` 및 `MySQL`을 사용하는 C++ 프로그램을 설명합니다. <br>
이 프로그램은 MySQL 데이터베이스와 상호작용하여 데이터를 삽입, 조회, 업데이트 및 삭제하는 간단한 CRUD(Create, Read, Update, Delete) 작업을 수행합니다. <br>
각 함수는 ACL 라이브러리의 기능을 활용하여 SQL 쿼리를 실행합니다. <br>

## 개요

이 프로그램은 MySQL과 상호작용하기 위해 ACL 라이브러리를 사용하며, `test_tbl`이라는 이름의 테이블을 생성하고, 데이터를 삽입하고, 조회하고, 업데이트하고, 삭제하는 기능을 제공합니다. <br>
프로그램은 사용자로부터 입력을 받아 CRUD 작업을 수행할 수 있도록 설계되었습니다. <br>
- [🖱️Acl Mysql 쿼리 예시 소스코드](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/db/mysql_query)

### 초기 설정
- [📄mysql with acl 문서 보러가기](../Document/MySQL-Acl.md)

### 주의할 부분
1. mysql 설치 및 libmysql 경로 확인하기 (acl 사용 시 필요)
   
2. mysql ssl 설정 관련 오류 발생 시, mysql의 my.ini의 mysqld 섹션에 ssl=0 추가하기 (ssl 설정 해제)
   
3. Acl 사용 시 `acl::query` 사용하기
   - [📄acl mysql 소스코드 주의사항](../Document/MySQL-Acl.md#acl-사용-시-주의사항)


## 소스코드 설명

- `lib_acl.h`와 `acl_cpp/lib_acl.hpp`는 ACL 라이브러리의 헤더 파일로, MySQL과의 데이터베이스 연결 및 쿼리 실행에 사용됩니다.

### 테이블 생성 SQL 쿼리

```cpp
const char* CREATE_TBL =
"CREATE TABLE IF NOT EXISTS test_tbl\r\n"
"(\r\n"
"id INT AUTO_INCREMENT PRIMARY KEY,\r\n"
"name VARCHAR(100) NOT NULL,\r\n"
"age INT NOT NULL\r\n"
")";
```

```sql
CREATE TABLE IF NOT EXISTS test_tbl(
id INT AUTO_INCREMENT PRIMARY KEY,
name VARCHAR(100) NOT NULL,
age INT NOT NULL
);
```

- `CREATE_TBL` 변수는 `test_tbl`이라는 테이블을 생성하는 SQL 명령문입니다.
- 이 테이블은 `id`, `name`, `age`라는 세 가지 필드를 가지고 있으며, `id`는 기본 키로 설정되어 있습니다.

### 함수 설명

#### `tbl_create`

이 함수는 `test_tbl` 테이블이 존재하지 않으면 테이블을 생성합니다.

```cpp
static bool tbl_create(acl::db_handle& db)
{
    if (db.tbl_exists("test_tbl")) {
        printf("Table 'test_tbl' already exists.\r\n");
        return true;
    }
    if (db.sql_update(CREATE_TBL) == false) {
        printf("SQL error while creating table.\r\n");
        return false;
    }
    printf("Table 'test_tbl' created successfully.\r\n");
    return true;
}
```

- `db.tbl_exists("test_tbl")`를 사용하여 테이블이 이미 존재하는지 확인합니다.
- 테이블이 존재하지 않으면 `CREATE_TBL` SQL 명령을 실행하여 테이블을 생성합니다.

#### `tbl_insert`

이 함수는 사용자로부터 받은 `name`과 `age` 값을 `test_tbl`에 삽입합니다.

```cpp
static bool tbl_insert(acl::db_handle& db, const char* name, int age)
{
    acl::query query;

    query.create_sql("INSERT INTO test_tbl(name, age) VALUES(:name, :age)")
        .set_parameter("name", name)
        .set_parameter("age", age);

    if (db.exec_update(query) == false) {
        printf("Insert SQL error: %s\r\n", db.get_error());
        return false;
    }

    printf("Data inserted successfully.\r\n");
    return true;
}
```

- `query.create_sql`을 통해 삽입할 SQL 쿼리를 생성합니다.
- `set_parameter`를 사용해 SQL 쿼리에서 사용할 값을 설정합니다.
- `db.exec_update(query)`

#### `tbl_select`

이 함수는 `test_tbl`의 모든 데이터를 조회하여 출력합니다.

```cpp
static bool tbl_select(acl::db_handle& db)
{
    acl::query query;
    query.create_sql("select * from test_tbl");

    if (db.exec_select(query) == false)
    {
        printf("select sql error\r\n");
        return false;
    }

    const acl::db_rows* result = db.get_result();
    if (result == NULL) {
        printf("No result found.\r\n");
        return false;
    }

    const std::vector<acl::db_row*>& rows = result->get_rows();
    if (rows.empty()) {
        printf("No data found.\r\n");
        return false;
    }

    for (size_t i = 0; i < rows.size(); i++) {
        const acl::db_row* row = rows[i];
        printf("ID: %s, Name: %s, Age: %s\r\n",
            row->field_value("id"), row->field_value("name"), row->field_value("age"));
    }

    db.free_result();
    return true;
}
```

- `exec_select`를 사용해 데이터를 선택하고 결과를 출력합니다.
- 조회된 데이터를 `db_rows`를 사용해 저장하고 이를 반복문으로 출력합니다.

#### `tbl_update`

이 함수는 주어진 `id`에 해당하는 레코드를 업데이트합니다.

```cpp
static bool tbl_update(acl::db_handle& db, int id, const char* new_name, int new_age)
{
    acl::query query;

    query.create_sql("UPDATE test_tbl SET name=:name, age=:age WHERE id=:id")
        .set_parameter("name", new_name)
        .set_parameter("age", new_age)
        .set_parameter("id", id);

    if (db.exec_update(query) == false) {
        printf("Update SQL error: %s\r\n", db.get_error());
        return false;
    }

    printf("Data updated successfully.\r\n");
    return true;
}
```

- `db.exec_update(query)`

#### `tbl_delete`

이 함수는 주어진 `id`에 해당하는 레코드를 삭제합니다.

```cpp
static bool tbl_delete(acl::db_handle& db, int id)
{
    acl::query query;

    query.create_sql("DELETE FROM test_tbl WHERE id=:id")
        .set_parameter("id", id);

    if (db.exec_update(query) == false) {
        printf("Delete SQL error: %s\r\n", db.get_error());
        return false;
    }

    printf("Data deleted successfully.\r\n");
    return true;
}
```

- `db.exec_update(query)`

### 메인 함수

메인 함수는 MySQL 서버에 연결한 후 사용자로부터 입력을 받아 CRUD 작업을 수행합니다.

```cpp
int main(void)
{
    acl::acl_cpp_init();
    acl::log::stdout_open(true);

    const char* libname = "libmysql.dll";
    acl::string path;
    path.format("C:\\Program Files\\MySQL\\MySQL Server 8.0\\lib\\%s", libname);
    acl::db_handle::set_loadpath(path);

    const char* dbaddr = "127.0.0.1:3306";
    const char* dbname = "testdb";
    const char* dbuser = "root";
    const char* dbpass = "000930";

    acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);

    if (db.open() == false) {
        printf("Failed to open database '%s'.\r\n", dbname);
        return 1;
    }

    printf("Connected to database '%s'.\r\n", dbname);

    // 테이블 생성
    if (tbl_create(db) == false) {
        return 1;
    }

    // 사용자 입력을 통해 CRUD 작업 선택
    char action;
    char name[100];
    int id, age;

    while (true) {
        printf("\nChoose an action: [i]nsert, [s]elect, [u]pdate, [d]elete, [q]uit: ");
        scanf(" %c", &action);

        switch (action) {
        case 'i':  // 데이터 삽입
            printf("Enter name: ");
            scanf("%s", name);
            printf("Enter age: ");
            scanf("%d", &age);
            tbl_insert(db, name, age);
            break;

        case 's':  // 데이터 조회
            printf("Select all data:\r\n");
            tbl_select(db);
            break;

        case 'u':  // 데이터 업데이트
            printf("Enter ID to update: ");
            scanf("%d", &id);
            printf("Enter new name: ");
            scanf("%s", name);
            printf("Enter new age: ");
            scanf("%d", &age);
            tbl_update(db, id, name, age);
            break;

        case 'd':  // 데이터 삭제
            printf("Enter ID to delete: ");
            scanf("%d", &id);
            tbl_delete(db, id);
            break;

        case 'q':  // 종료
            printf("Exiting...\r\n");
            return 0;

        default:
            printf("Invalid action. Try again.\r\n");
            break;
        }
    }

    return 0;
}
```

### 프로그램 실행 흐름

1. 데이터베이스에 연결하고 `test_tbl` 테이블을 생성합니다.
2. 사용자가 입력한 작업에 따라 데이터를 삽입, 조회, 업데이트 또는 삭제할 수 있습니다.
3. `q`를 입력하면 프로그램이 종료됩니다.

