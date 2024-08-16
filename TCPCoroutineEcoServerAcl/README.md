# Acl을 활용한 Coroutine TCP Eco Server 예제

## Acl - Coroutine
- [fiber lib 공식문서 설명](https://github.com/acl-dev/acl/blob/master/lib_fiber/README_en.md)
- **코루틴**은 비동기 프로그래밍을 간편하게 만들어주는 도구입니다. <br>
일반적인 스레드와는 달리, 하나의 스레드 내에서 여러 코루틴을 생성하고 실행할 수 있으며, <br>
이들 코루틴은 서로 독립적으로 실행됩니다.<br>
코루틴을 사용하면 프로그램의 흐름을 중단하지 않고 비동기 작업을 수행할 수 있습니다.

- **코루틴의 장점**:
  - **비동기 처리**: 비동기적으로 여러 작업을 처리할 수 있어 CPU 자원을 효율적으로 사용할 수 있습니다.
  - **쉬운 동시성 구현**: 복잡한 스레드 관리 없이 간단한 코드로 동시성을 구현할 수 있습니다.
  - **경량 스레드**: 스레드보다 더 적은 자원을 사용하여 효율적인 비동기 작업을 수행할 수 있습니다.

--------------------

## 예제
: ACL 라이브러리의 코루틴 기능을 사용하여 TCP 서버를 구현한 것입니다. 

### 1. `run_tcp_coroutine_server` 함수의 정의

```cpp
void run_tcp_coroutine_server() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;
    if (!server.open(addr)) {
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return;
    }

    std::cout << "Coroutine TCP Server is running on " << addr << std::endl;
```

- **서버 소켓 생성**: `acl::server_socket server;` 서버 소켓 객체를 생성 (이 소켓은 클라이언트의 연결을 기다림)
- **소켓 열기**: `server.open(addr)`를 통해 지정된 주소(`127.0.0.1:8088`)에서 서버 소켓을 열어 클라이언트 연결을 받을 준비 시작

### 2. 코루틴을 사용한 클라이언트 연결 처리

```cpp
    go[&]{
        while (true) {
            acl::socket_stream* conn = server.accept();
            if (conn) {
                go[=] {
                    char buf[256];
                    while (true) {
                        int ret = conn->read(buf, sizeof(buf), false);
                        if (ret <= 0) {  // 데이터 읽기 실패 또는 연결 종료 시
                            std::cerr << "Failed to read data from client or connection closed." << std::endl;
                            break;
                        }

                        buf[ret] = '\0';  // 문자열로 처리하기 위해 null-terminator 추가
                        std::cout << "Received from client: " << buf << std::endl;

                        if (conn->write(buf, ret) != ret) {
                            std::cerr << "Failed to send data to client." << std::endl;
                            break;
                        }
                    }
                    delete conn;  // 연결 종료 시 소켓 스트림 삭제
                };
            }
        }
    };
```

#### 2.1 **최초 코루틴 생성 (서버 역할)**

- **코루틴 시작**: `go[&]{}`는 코루틴을 시작하는 구문
  + 코루틴은 비동기적 작업을 쉽게 구현할 수 있게 해주는 경량 스레드입니다.
  + 여기서 `&`는 캡처 리스트로, 코루틴 내부에서 외부의 `server` 객체를 사용 가능하도록 함

- **클라이언트 연결 수락**: `server.accept()`는 클라이언트의 연결을 기다리며, 연결이 수락되면 `acl::socket_stream* conn` 객체를 반환
  + 이 객체를 사용해 클라이언트와 데이터를 주고받음

#### 2.2 **클라이언트마다의 코루틴 생성**

- **새로운 클라이언트 코루틴**: `go[=]{}`를 사용하여 클라이언트가 연결될 때마다 새로운 코루틴 생성
  + 이 코루틴은 클라이언트의 요청을 처리하고 응답을 보내는 역할
  + `=`는 캡처 리스트로, 외부 변수(여기서는 `conn`)을 복사하여 코루틴 내에서 사용할 수 있도록 함

#### 2.3 **클라이언트 데이터 처리**

- **데이터 읽기 및 에코**: `conn->read(buf, sizeof(buf), false)`를 통해 클라이언트로부터 데이터를 읽습니다.
  + 읽은 데이터는 `buf`에 저장되고, 이를 다시 클라이언트로 재전송
  
  - **데이터 읽기 오류 처리**: `ret <= 0` 조건은 데이터 읽기 실패 또는 클라이언트가 연결을 끊은 경우를 의미
    + 이때 코루틴은 `break`를 통해 루프를 탈출하고, 연결을 종료합니다.

- **문자열 처리**: 데이터를 문자열로 처리하기 위해 `buf[ret] = '\0';`을 사용해 null-terminator 추가

- **에코 실패 처리**: 만약 `conn->write`에서 실패하면 오류 메시지를 출력하고 연결을 종료합니다.

- **자원 해제**: 마지막으로 `delete conn;`을 통해 연결된 클라이언트 소켓 객체를 삭제하여 메모리를 해제합니다.

### 3. 코루틴 스케줄러 시작

```cpp
    acl::fiber::schedule();  // 코루틴 스케줄러 시작
}
```

- **코루틴 스케줄러 실행**: `acl::fiber::schedule()`을 호출하여 코루틴 스케줄링을 시작합니다. 이 함수는 코루틴이 일정하게 실행되도록 관리합니다.

### 4. 코루틴 개념 관련 설명

- **코루틴**은 비동기 프로그래밍을 간편하게 만들어주는 도구입니다. 일반적인 스레드와는 달리, 하나의 스레드 내에서 여러 코루틴을 생성하고 실행할 수 있으며, 이들 코루틴은 서로 독립적으로 실행됩니다. 코루틴을 사용하면 프로그램의 흐름을 중단하지 않고 비동기 작업을 수행할 수 있습니다.

- **코루틴의 장점**:
  - **비동기 처리**: 비동기적으로 여러 작업을 처리할 수 있어 CPU 자원을 효율적으로 사용할 수 있습니다.
  - **쉬운 동시성 구현**: 복잡한 스레드 관리 없이 간단한 코드로 동시성을 구현할 수 있습니다.
  - **경량 스레드**: 스레드보다 더 적은 자원을 사용하여 효율적인 비동기 작업을 수행할 수 있습니다.

이 코드에서 각 클라이언트는 별도의 코루틴에서 처리되므로, 여러 클라이언트가 동시에 연결되어도 효율적으로 동작할 수 있습니다.






















