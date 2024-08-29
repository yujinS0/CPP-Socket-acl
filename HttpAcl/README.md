# HTTP Module acl
Acl에서 제공하는 HTTP Module을 활용한 Http Server 코드

<br><br>


## 기본 Sample 코드 : http_ex.cpp 코드 분석
`http_ex.cpp` 은 acl에서 제공하는 http 모듈을 활용한 기본적인 예시 코드입니다.

<br>

```cpp
#include "acl_cpp/lib_acl.hpp"  // Must before http_server.hpp
#include "fiber/http_server.hpp"
```
- `acl_cpp/lib_acl.hpp`: ACL C++ 라이브러리의 기본 헤더를 포함합니다. 이 헤더는 네트워킹, 파일 처리, 문자열 처리 등 다양한 기능을 제공합니다.
- `fiber/http_server.hpp`: HTTP 서버를 구성하는 데 필요한 클래스를 정의한 헤더 파일입니다.

```cpp
static char* var_cfg_debug_msg;
static acl::master_str_tbl var_conf_str_tab[] = {
  { "debug_msg", "test_msg", &var_cfg_debug_msg },
  { 0, 0, 0 }
};

static int  var_cfg_io_timeout;
static acl::master_int_tbl var_conf_int_tab[] = {
  { "io_timeout", 120, &var_cfg_io_timeout, 0, 0 },
  { 0, 0 , 0 , 0, 0 }
};
```
- **설정 변수 초기화**: `var_cfg_debug_msg`와 `var_cfg_io_timeout`은 서버의 설정 값을 저장하는 변수입니다.
- `acl::master_str_tbl` 및 `acl::master_int_tbl` 구조체 배열을 통해 문자열과 정수 설정을 저장하고 있습니다. 이러한 설정 값은 서버가 초기화될 때 로드됩니다.

```cpp
int main() {
    const char* addr = "0.0.0.0|8194";
    const char* conf = NULL;

    acl::acl_cpp_init();
```
- `addr`: 서버가 바인딩할 IP 주소와 포트를 나타냅니다. `0.0.0.0`은 모든 네트워크 인터페이스에서 요청을 수신하는 것을 의미하며, `8194`는 서버가 리스닝할 포트 번호입니다.
- `conf`: 서버 설정 파일을 지정할 수 있지만, 여기서는 사용하지 않습니다.
- `acl::acl_cpp_init()`: ACL 라이브러리의 초기화를 수행합니다.

```cpp
    acl::http_server server;

    // Call the methods in acl::master_base class.
    server.set_cfg_int(var_conf_int_tab).set_cfg_str(var_conf_str_tab);
```
- `acl::http_server`: HTTP 서버 객체를 생성합니다.
- `set_cfg_int`, `set_cfg_str`: 초기화 단계에서 설정된 정수와 문자열 값을 서버 객체에 설정합니다.

```cpp
    server.on_proc_init([&addr] {
        printf("---> after process init: addr=%s, io_timeout=%d\r\n", addr, var_cfg_io_timeout);
    }).on_proc_exit([] {
        printf("---> before process exit\r\n");
    }).on_proc_sighup([](acl::string& s) {
        s = "+ok";
        printf("---> process got sighup\r\n");
        return true;
    }).on_thread_accept([](acl::socket_stream& conn) {
        printf("---> accept %d\r\n", (long long)conn.sock_handle());
        return true;
    })
```
- **이벤트 핸들러 등록**: 서버의 수명 주기 동안 특정 이벤트가 발생할 때 호출되는 핸들러들을 설정합니다.
    - `on_proc_init`: 서버 프로세스가 초기화된 후 호출됩니다.
    - `on_proc_exit`: 서버 프로세스가 종료되기 전에 호출됩니다.
    - `on_proc_sighup`: `SIGHUP` 시그널을 수신했을 때 호출됩니다.
      + `SIGHUP` 시그널 : 통신 접속이 끊어졌을 때 프로세스들에게 보내는 시그널
    - `on_thread_accept`: 클라이언트 연결이 수락되었을 때 호출됩니다.

```cpp
    .Get("/", [](acl::HttpRequest&, acl::HttpResponse& res) {
        std::string buf("hello world1!\r\n");
        res.setContentLength(buf.size());
        return res.write(buf.c_str(), buf.size());
    }).Post("/json", [](acl::HttpRequest& req, acl::HttpResponse& res) {
        acl::json* json = req.getJson();
        if (json) {
            return res.write(*json);
        } else {
            std::string buf = "no json got\r\n";
            res.setContentLength(buf.size());
            return res.write(buf.c_str(), buf.size());
        }
    })
```
- **HTTP 요청 핸들러 등록**: 특정 경로에 대한 HTTP 메서드(GET, POST 등) 요청을 처리하는 핸들러들을 설정합니다.
    - `Get("/")`: 루트 경로로 GET 요청이 들어오면 "hello world1!" 메시지를 반환합니다.
    - `Post("/json")`: "/json" 경로로 POST 요청이 들어오면 JSON 데이터를 처리하고 반환합니다.

```cpp
    .run_alone(addr, conf);

    return 0;
}
```
- `run_alone`: 서버를 지정된 주소와 포트에서 단독으로 실행합니다.

