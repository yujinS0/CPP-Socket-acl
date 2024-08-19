# 예제 코드&설명 모음
* [lib_fiber 예제 소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c++)

<br>

### [빠른 시작](https://github.com/acl-dev/acl?tab=readme-ov-file#4-quick-start)
* The first demo with Acl
  
* Simple TCP server
    ```cpp 
    #include <thread>
    #include "pch.h"
    
    void run(void) {
        const char* addr = "127.0.0.1:8088";
        acl::server_socket server; // server_socket 클래스 인스턴스 생성
        if (!server.open(addr)) {  // addr 주소로 서버 소켓을 열고, Bind해서 클라이언트의 연결 요청을 대기 상태(listen)로 만들기
            return; // 실패 시 바로 종료
        }
    
        while (true) {
            acl::socket_stream* conn = server.accept(); // Wait for connection. 클라이언트의 연결 요청 대기 
                                                        // -> 만약 클라이언트가 연결 시도하면, 새로운 socket_stream 객체 반환
            if (conn == NULL) {
                break;
            }
            std::thread thread([=] {  // Start one thread to handle the connection (스레드 생성)
                char buf[256];  // 최대 256 바이트 데이터
                int ret = conn->read(buf, sizeof(buf), false);  // Read data 
                if (ret > 0) {
                    conn->write(buf, ret);  // 읽은 데이터 다시 전송
                }
                delete conn; // 연결 끝나면, socket_stream 객체 삭제
                });
            thread.detach();  // 생성된 스레드 detach (생성된 스레드는 메인 스레드와 독립적으로 실행)
                              // => 메인 스레드가 종료되어도 스레드가 독립적으로 계속 실행.
        }
    }

    
    ```
  
<br>

* Simple TCP client

<br>

* Coroutine TCP server  [코루틴이란?](./Coroutine.md)
    
    ```cpp 
    #include "acl_cpp/lib_acl.hpp"
    #include "fiber/go_fiber.hpp"
    
    void run(void) {
      const char* addr = "127.0.0.1:8088";
      acl::server_socket server;
      if (!server.open(addr)) {
        return;
      }
    
      go[&] {  // Create one server coroutine to wait for connection.
        while (true) {
          acl::socket_stream* conn = server.accept();
          if (conn) {
            go[=] {  // Create one client coroutine to handle the connection.
              char buf[256];
              int ret = conn->read(buf, sizeof(buf), false);
              if (ret > 0) {
                (void) conn->write(buf, ret);
              }
              delete conn;
            };
          }
        }
      };
    
      acl::fiber::schedule();  // Start the coroutine scheculde process.
    }

    
    ```

<br>

### [Server-Client 테스트 코드](https://github.com/acl-dev/acl/blob/master/lib_fiber/README_en.md#samples)
* Coroutine server with c++
* Coroutine server with c++11
* Windows GUI sample
