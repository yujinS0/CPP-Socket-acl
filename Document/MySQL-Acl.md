# Acl을 활용해 MySQL 쉽게 접근하기

### Acl 빌드 및 관련 정적 라이브러리 추가
Acl 사용을 위해 당연히 추가해줘야한다.
[ACL 빌드 및 정적 라이브러리 추가 방법](https://github.com/yujinS0/CPP-Socket-acl/tree/main?tab=readme-ov-file#acl-빌드-및-정적-라이브러리로-추가하는-방법)

- [오픈소스 라이브러리(Acl 빌드 후) 정적 라이브러리로 추가 방법](./Build.md)
- [친절한 정적 라이브러리 추가 방법](./HowToAddCPPStaticLibrary.md)


### mysql 관련 라이브러리 추가

우선 Acl을 사용하더라도, mysql 연결을 해주는 libmysql 자체는 따로 설치(추가)를 해줘야한다. <br>
테스트를 위해서도 MySQL은 필요하니 공식 홈페이지에서 MySQL Workbench나 MySQL Server 설치를 진행하자.  <br><br>
이때, MySQL 공식 홈페이지에서 다운 받을 때 Server까지 설치해주면, <br>
`C:\Program Files\MySQL\MySQL Server 8.0\include` 해당 경로에 이미 **헤더파일**이 있고, <br>
`C:\Program Files\MySQL\MySQL Server 8.0\lib` 에는 **정적 라이브러리(.lib)**, **동적 라이브러리(.dll)** 파일이 있을 것이다. <br>


 <br> <br>


### Visual Studio에서 MySQL C API 사용을 위한 프로젝트 설정 방법 (Windows 환경)

#### 1. **라이브러리 및 헤더 파일 설정**

1. **헤더 파일 경로 추가**:
   - Visual Studio에서 프로젝트를 열고, 프로젝트 속성 창을 엽니다.
   - **C/C++ -> 일반 -> 추가 포함 디렉터리**로 이동합니다.
   - MySQL Server 설치 경로 내의 `include` 폴더 (`C:\Program Files\MySQL\MySQL Server 8.0\include`)를 추가합니다.

2. **라이브러리 파일 경로 추가**:
   - 같은 속성 창에서 **링커 -> 일반 -> 추가 라이브러리 디렉터리**로 이동합니다.
   - MySQL Server 설치 경로 내의 `lib` 폴더 (`C:\Program Files\MySQL\MySQL Server 8.0\lib`)를 추가합니다.
   
3. **링커 입력 설정**:
   - **링커 -> 입력 -> 추가 종속성** 항목에 `libmysql.lib`를 추가합니다.

#### 2. **libmysql.dll 설정**

1. MySQL 클라이언트 라이브러리인 `libmysql.dll` 파일을 실행 파일 디렉토리로 복사해야 합니다. 이는 프로젝트가 MySQL과 통신할 때 필요합니다. 
   - `C:\Program Files\MySQL\MySQL Server 8.0\lib` 디렉토리 내에 `libmysql.dll` 파일이 있습니다.
   - 이를 **실행 파일 디렉토리**로 복사합니다. ex. Debug 디렉토리

<br>

### Acl mysql 사용을 위해
1. `libmysql.lib` 경로 확인 (C:\Program Files\MySQL\MySQL Server 8.0\lib)
   - acl에서 libmysql를 사용하기 때문에 추후 소스코드에서 경로를 지정해줘야 합니다.
2. `libmysql.dll` 파일의 위치를 실행파일과 같은 디렉토리로 이동



 <br> <br>
 

## acl 사용 시 주의사항

acl에서 mysql 쿼리문을 다루는 방법이 두가지가 존재합니다.
1. `acl::string` 사용해서 raw 쿼리문 지정 및 `sql_update(query)`로 업데이트 시키기
2. `acl::query` 사용해서 파라미터 지정 및 `exec_update(query)` 로 업데이트 시키기

결론적으로 **2번 방법인 acl::query**를 사용해야만 합니다!

### 1. acl::string & sql_* 함수(또는 get_result)사용

```cpp
static bool tbl_select(acl::db_handle& db)
{
    const char* sql = "SELECT * FROM test_tbl";

    bool isSelect = db.sql_select(sql);
    if (isSelect == false) {
        printf("Select SQL error: %s\r\n", db.get_error());
        return false;
    }

    const acl::db_rows* result = db.get_result();

    if (result == NULL) {
        printf("No result found. Result pointer is NULL.\r\n");
        db.free_result();
        return false;
    }

    const std::vector<acl::db_row*>& rows = result->get_rows();
    if (rows.empty()) {
        printf("No data found.\r\n");
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

- 정상적으로 작동하는 것처럼 보여도 `db.get_result()` 시 NULL이 되어서 정상적으로 오류 처리를 하기 힘듦.
- 또한 raw 쿼리문을 작성하면 사실 상 MySQL이 제공하는 C API 사용과 큰 다른 점이 없음.


### 2. acl::query & exec_* 함수 사용


```cpp

static bool tbl_select(acl::db_handle& db)
{
    acl::query query;
    query.create_sql("select * from test_tbl")
        .set_format("id", "id")
        .set_format("name", "name")
        .set_format("age", "age");

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

- acl::query 사용 시 `.set_parameter` 또는 `.set_format` 로 안전하게 SQL 인젝션 공격을 막고 가독성을 높일 수 있음.
- 이때 exec_update로 업데이트를 진행하면 ture false 반환 값도 정상적으로 나옴을 확인할 수 있음.











