# HTTP Module ACL: HTTP 서버 코드 분석

## 목차

1. [기본 Sample 코드: `http_ex.cpp` 분석](#기본-sample-코드-http_excpp-분석)
2. [코드 구성 요소](#코드-구성-요소)
    - [헤더 파일 포함](#헤더-파일-포함)
    - [설정 변수 초기화](#설정-변수-초기화)
    - [서버 초기화](#서버-초기화)
3. [이벤트 핸들러 설정](#이벤트-핸들러-설정)
4. [HTTP 요청 처리 핸들러](#http-요청-처리-핸들러)
5. [서버 실행](#서버-실행)
6. [비동기 처리 및 코루틴](#비동기-처리-및-코루틴)
    - [1. 클라이언트 연결 수락 (`on_accept`)](#1-클라이언트-연결-수락-on_accept)
    - [2. HTTP 요청 처리 (`http_servlet::doRun`)](#2-http-요청-처리-http_servletdorun)
    - [3. 서버 실행 흐름](#3-서버-실행-흐름)
        - [3.1 `run_alone` 함수](#31-run_alone-함수)
        - [3.2 `run` 함수](#32-run-함수)
        - [3.3 `acl_fiber_server_main` 함수](#33-acl_fiber_server_main-함수)
        - [3.4 클라이언트 연결 수락 (`service_on_accept` 함수)](#34-클라이언트-연결-수락-service_on_accept-함수)
        - [3.5 클라이언트 연결 처리 (`on_accept` 함수)](#35-클라이언트-연결-처리-on_accept-함수)
        - [3.6 HTTP 요청 처리 (`http_servlet::doRun` 함수)](#36-http-요청-처리-http_servletdorun-함수)
    - [4. 비동기적으로 처리하는 서버의 전체 흐름 요약](#4-비동기적으로-처리하는-서버의-전체-흐름-요약)
7. [결론](#결론)
8. [참고 문서](#참고-문서)

### 기본 Sample 코드: `http_ex.cpp` 분석

`http_ex.cpp`은 ACL에서 제공하는 HTTP 모듈을 활용한 기본적인 예시 코드입니다. 이 코드에서는 ACL C++ 라이브러리를 사용하여 HTTP 서버를 설정하고 실행하는 방법을 보여줍니다.

<br>


### 코드 구성 요소

```cpp
#include "acl_cpp/lib_acl.hpp"  // Must before http_server.hpp
#include "fiber/http_server.hpp"
```
- **`acl_cpp/lib_acl.hpp`**: ACL C++ 라이브러리의 기본 헤더입니다. 다양한 네트워킹, 파일 처리, 문자열 처리 기능을 제공합니다.
- **`fiber/http_server.hpp`**: HTTP 서버를 구성하는 데 필요한 클래스를 정의합니다.

```cpp
static char* var_cfg_debug_msg;
static acl::master_str_tbl var_conf_str_tab[] = {
  { "debug_msg", "test_msg", &var_cfg_debug_msg },
  { 0, 0, 0 }
};

static int var_cfg_io_timeout;
static acl::master_int_tbl var_conf_int_tab[] = {
  { "io_timeout", 120, &var_cfg_io_timeout, 0, 0 },
  { 0, 0 , 0 , 0, 0 }
};
```
- **설정 변수 초기화**: `var_cfg_debug_msg`와 `var_cfg_io_timeout`은 서버 설정 값을 저장하는 변수입니다. 이들은 `acl::master_str_tbl` 및 `acl::master_int_tbl` 구조체 배열을 통해 서버 설정 시 사용됩니다.

```cpp
int main() {
    const char* addr = "0.0.0.0|8194";
    const char* conf = NULL;

    acl::acl_cpp_init();

    acl::http_server server;

    server.set_cfg_int(var_conf_int_tab).set_cfg_str(var_conf_str_tab);
```
- **서버 초기화**: `acl_cpp_init()`을 통해 ACL 라이브러리를 초기화하고, `http_server` 객체를 생성하여 설정을 적용합니다.
- **`addr`**: 서버가 바인딩할 IP 주소와 포트를 나타냅니다.

<br>


### 이벤트 핸들러 설정

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
    + 서버 설정을 확인하거나 초기 상태를 출력하는 데 사용됩니다.
  - `on_proc_exit`: 서버 프로세스가 종료되기 전에 호출됩니다.
    + 종료 직전에 필요한 정리 작업을 수행합니다.
  - `on_proc_sighup`: `SIGHUP` 시그널을 수신했을 때 호출됩니다.
    + 보통 설정을 다시 로드하거나 서버를 재시작하는 데 사용됩니다.
  - `on_thread_accept`: 클라이언트 연결이 수락되었을 때 호출됩니다.
    + 이때 수락된 연결에 대해 추가적인 설정이나 초기화 작업을 수행할 수 있습니다.

<br>


### HTTP 요청 처리 핸들러

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
- **HTTP 요청 핸들러 등록**: GET 및 POST 요청에 대한 핸들러를 설정합니다.
  - `Get("/")`: 루트 경로로 GET 요청이 들어오면 "hello world1!" 메시지를 반환합니다.
  - `Post("/json")`: "/json" 경로로 POST 요청이 들어오면 JSON 데이터를 처리하고 반환합니다.

<br>


### 서버 실행

```cpp
    .run_alone(addr, conf);

    return 0;
}
```
- **`run_alone`**: 서버를 지정된 주소와 포트에서 단독 실행합니다. 이 메서드는 서버를 비동기로 실행하며, 클라이언트의 요청을 처리하는 메인 루프를 시작합니다.

<br>


### 비동기 처리 및 코루틴

`master_fiber` 클래스와 이를 상속받은 `http_server_impl` 클래스는 비동기 처리 및 코루틴을 통해 서버가 다수의 클라이언트 요청을 동시에 처리할 수 있도록 설계되었습니다. 이 구조는 다음과 같이 동작합니다:

1. **클라이언트 연결 수락 (`on_accept`)**:
   - 클라이언트의 연결을 비동기적으로 수락하고, `http_servlet` 객체를 통해 각 요청을 처리합니다.
   
2. **HTTP 요청 처리 (`http_servlet::doRun`)**:
   - 각 요청을 비동기적으로 처리하며, 코루틴을 활용하여 I/O 작업 중에도 다른 요청을 처리할 수 있도록 합니다.

3. **서버 실행 흐름**:
   - `run_alone` → `run` → `acl_fiber_server_main` → `service_on_accept` → `http_servlet::doRun`의 순서로 서버가 비동기적으로 실행됩니다.
  


  
  #### `run_alone` 함수

  `run_alone` 함수는 서버 실행을 위한 준비를 마친 후, `run` 함수를 호출하여 서버의 비동기 실행 루프를 시작합니다.
  
  ```cpp
  bool master_fiber::run_alone(const char* addrs, const char* path /* = NULL */) {
      int argc = 0;
      const char *argv[9];
      const char *file_path = acl_process_path();
      argv[argc++] = file_path ? file_path : "unknown";
      argv[argc++] = "-L";
      argv[argc++] = addrs;
      if (path && *path) {
          argv[argc++] = "-f";
          argv[argc++] = path;
      }
  
      // 비동기 서버 실행을 시작
      run(argc, (char **) argv);
      return true;
  }
  ```

  - **설명**: 이 함수는 서버를 실행하기 위한 설정을 구성하고, `run` 함수를 호출하여 서버의 비동기 실행을 시작합니다.

  #### 3. `run` 함수

  `run` 함수는 `acl_fiber_server_main`을 호출하여 서버의 메인 비동기 루프를 시작합니다.
  
  ```cpp
  void master_fiber::run(int argc, char** argv) {
      // 서버의 메인 비동기 루프 시작
      acl_fiber_server_main(argc, argv, service_on_accept, this,
          /* ... 다양한 콜백 함수들 ... */);
  }
  ```
  
  - **설명**: `run` 함수는 `acl_fiber_server_main`을 호출하여 서버 실행의 중심이 되는 이벤트 루프를 설정합니다. 이 루프는 클라이언트 요청을 비동기적으로 처리할 준비를 합니다.
  
  #### 4. `acl_fiber_server_main` 함수
  
  이 함수는 서버의 메인 이벤트 루프를 설정하고, 비동기 서버 실행을 위한 준비를 합니다.
  
  ```cpp
  void acl_fiber_server_main(int argc, char *argv[],
      void (*service)(void*, ACL_VSTREAM*), void *ctx, int name, ...) {
      // 서버의 이벤트 루프를 시작
      servers_start(__servers, acl_var_fiber_threads);
  }
  ```
  
  - **설명**: `acl_fiber_server_main` 함수는 서버의 비동기 실행을 위한 메인 이벤트 루프를 설정하고, 여러 스레드 또는 코루틴에서 클라이언트의 연결을 처리할 수 있도록 준비합니다.
  
  #### 5. 클라이언트 연결 수락 (`service_on_accept` 함수)
  
  클라이언트 연결이 수락될 때, `service_on_accept` 함수가 호출되어 클라이언트 연결을 처리합니다.
  
  ```cpp
  void master_fiber::service_on_accept(void* ctx, ACL_VSTREAM *client) {
      master_fiber* mf = (master_fiber *) ctx;
      acl_assert(mf != NULL);
  
      socket_stream* stream = new socket_stream;
      if (!stream->open(client)) {
          logger_error("open stream error(%s)", acl_last_serror());
          delete stream;
          return;
      }
  
      // 클라이언트 연결을 처리
      mf->on_accept(*stream);
      stream->unbind(); // 연결을 unbind 하여 소켓 스트림을 더 이상 참조하지 않게 함
      delete stream; // 소켓 스트림 객체를 삭제하여 자원을 해제
  }
  ```
  
  - **설명**:
    + 이 함수는 새로운 클라이언트 연결이 발생할 때 호출됩니다.
    + `socket_stream` 객체를 생성하여 클라이언트의 연결을 처리하고, 해당 객체를 `on_accept` 메서드에 전달하여 비동기적으로 처리합니다.
  
  #### 6. 클라이언트 연결 처리 (`on_accept` 함수)
  
  `on_accept` 함수는 클라이언트의 요청을 처리하기 위해 `http_servlet` 객체를 생성하고, 요청을 처리하는 루프를 실행합니다.
  
  ```cpp
  void master_fiber::on_accept(socket_stream& conn) {
      acl::session* session = new memcache_session("127.0.0.1|11211");
      http_servlet servlet(handlers_, &conn, session);
      servlet.setLocalCharset("utf-8");
  
      // HTTP 요청을 처리
      while (servlet.doRun()) {}
  
      delete session;
  }
  ```
  
  - **설명**:
    + `on_accept` 함수는 클라이언트 연결을 처리하는 핵심 함수입니다.
    + 각 클라이언트에 대해 독립적인 `http_servlet` 객체가 생성되며, 이 객체가 요청을 처리합니다. 이를 비동기적으로 관리합니다.
  
  #### 7. HTTP 요청 처리 (`http_servlet::doRun` 함수)
  
  `http_servlet::doRun` 함수는 실제로 HTTP 요청을 처리하고, 클라이언트에게 응답을 전송합니다.
  
  ```cpp
  bool http_servlet::doRun() {
      acl::HttpRequest request;
      acl::HttpResponse response;
  
      if (parseRequest(request) && handlers_[request.getMethod()][request.getPath()]) {
          handlers_[request.getMethod()][request.getPath()](request, response);
      }
  
      response.send(conn_);
      return conn_->keep_alive();
  }
  ```
  
  - **설명**:
    + `doRun` 메서드는 HTTP 요청을 처리하는 데 사용됩니다.
    + 요청이 들어오면 이를 파싱하고, 해당 요청에 맞는 핸들러를 실행하여 응답을 생성합니다.
    + 생성한 응답은 send 메서드를 통해 클라이언트로 전송됩니다.
  
  #### 비동기적으로 처리하는 서버의 전체 흐름 요약
  
  
  1. **`run_alone`**: 서버 실행 준비 및 비동기 실행 시작.
  2. **`run`**: 서버의 메인 비동기 루프 시작.
  3. **`acl_fiber_server_main`**: 서버의 비동기 이벤트 루프 설정.
  4. **`service_on_accept`**: 클라이언트 연결 수락 및 비동기 처리 준비.
  5. **`on_accept`**: 클라이언트의 HTTP 요청 처리 시작.
  6. **`http_servlet::doRun`**: HTTP 요청을 비동기적으로 처리하고, 클라이언트에게 응답 전송.
  
  이 전체 과정에서 코루틴을 통해 서버는 다수의 클라이언트 요청을 동시에 처리할 수 있으며, I/O 작업 중에도 다른 요청을 처리할 수 있는 구조를 갖추게 됩니다.
  

<br>


### 결론

- 이 코드에서의 비동기 처리의 핵심은 **코루틴**을 통한 **비동기 I/O**입니다. 
  + `run_alone` 함수에서 시작된 비동기 서버 실행은 클라이언트 연결 수락과 요청 처리에서 코루틴을 사용하여 다수의 요청을 동시에 처리할 수 있게 합니다. 
  + 이로 인해 서버는 높은 성능을 유지하며, 여러 클라이언트를 동시에 처리할 수 있습니다.

<br>


### 참고 문서

- **[📄Acl HTTP Server 기능 정리](../Document/Acl-HTTP-Server.md)**: ACL의 HTTP 모듈에 대한 더 자세한 기능과 사용법을 다루고 있습니다.

