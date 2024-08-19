# Acl - Coroutine (코루틴)

**코루틴**은 비동기 프로그래밍을 간편하게 만들어주는 도구입니다. <br>
일반적인 스레드와는 달리, 하나의 스레드 내에서 여러 코루틴을 생성하고 실행할 수 있으며, <br>
이들 코루틴은 서로 독립적으로 실행됩니다.<br>
코루틴을 사용하면 프로그램의 흐름을 중단하지 않고 비동기 작업을 수행할 수 있습니다.

<br>

## 코루틴(Coroutine)이란?

‘Co’routine(Co- + routine)으로 일종의 가벼운 스레드(Light-weight thread)로 동시성 작업을 간편하게 할 수 있게 해주는 역할을 합니다. <br>
### `즉, 코루틴은 실행의 지연과 재개를 허용함으로써,`
### `비선점적인 멀티태스킹을 위한 서브 루틴을 일반화한 컴퓨터 프로그램 구성요소라 볼 수 있습니다.` <br><br>


**비선점적인 멀티태스킹** <br>
+ **비선점형** : 다른 태스크가 끝나지 않으면 다른 프로세스는 CPU를 사용할 수 없음
  - CPU에 A프로세스가 있다면, A의 상태가 자체적으로 bolck되거나 terminated 되어야만 B를 실행할 수 있음.
  - 즉, 스케쥴러가 CPU에 있는 프로세스를 선점, 제어할 수 없음
  - 코루틴
+ **선점형** : 하나의 프로세스가 다른 프로세스 대신에 CPU를 차지할 수 있음
  - running인 상태여도 스케쥴러가 이를 중단 (ready) 시키고 다른 프로세스로 교체할 수 있음
  - 스레드

<br>

### `따라서 코루틴은 병행성(동시성)은 제공하지만, 병렬성은 제공하지 않는다. `
* 병행성(동시성) = 논리적으로 작업이 병렬로 실행되는 것처럼 보이는 것
* 병렬성 = 물리적으로 작업이 병렬로 실행되는 것



<br>


선점형으로 비동기 처리 시 아래와 같은 문제가 발생
- 코드의 복잡성 : 독립적인 스레드에서 각 루틴이 동작하므로 동시성 제어를 위한 복잡한 코드가 필요하고 코드 흐름 파악이 어렵다.
- Context switching 비용 : 스레드 사용시 Context switching 리소스 비용으로 성능 저하가 발생할 수 있다.

<br>

이런 문제 해결을 위해 비선점적 멀티 태스킹을 사용한 언어적 지원 방법이 **코루틴**입니다.

<br>


코루틴의 장점:
  - **쉽고 가독성 높은 비동기 처리**
    + 코루틴을 사용하면 예측 가능한 프로그래밍을 할 수 있기 때문에, 비교적 코드 흐름 파악이 쉬어 비동기 처리임에도 개발자가 가독성 높게 여러 작업을 처리할 수 있습니다.
  - **동시성 구현 및 경량 스레드**
    + 동시성 프로그래밍(2개 이상의 프로세스가 동시에 계산을 진행하는 상태)
    + 경량 스레드: 스레드보다 더 적은 자원을 사용하여 효율적인 비동기 작업을 수행할 수 있습니다.

---------------------------------
## `lib_fiber`

* acl의 `lib_fiber`는 windows 기준 **select, poll, iocp, GUI Message** 지원
* libfiber를 사용하면 이벤트 중심 모델을 사용하는 기존 비동기 프레임워크보다 <br> **고성능과 대용량 네트워크 애플리케이션 서비스를 더 쉽게 작성 가능**
* 또한 코루틴 방식으로 Windows에서 GUI 프레임워크(MFC, wtl 등)로 작성된 Windows GUI 앱의 네트워크 모듈도 작성 가능

<br>

* 여러 플랫폼 (Linux, MacOS, Windows, iOS 및 Android)에서 실행 가능
* x86, Arm 아키텍처 지원
* select/poll/epoll/kqueue/iocp/win32 GUI 메시지 지원
* DNS 프로토콜은 acl 코루틴으로 구현되었으므로 코루틴 모델에서도 DNS API를 사용할 수 있습니다.
* Unix 및 Windows에서 시스템 IO API 후킹
  + 읽기 API: read/readv/recv/recvfrom/recvmsg
  + 쓰기 API: write/writev/send/sendto/sendmsg/sendfile64
  + **소켓 API: socket/listen/accept/connect/setsockopt**
  + 이벤트 API: select/poll/epoll_create/epoll_ctl/epoll_wait
  + DNS API: gethostbyname/gethostbyname_r/getaddrinfo/freeaddrinfo
* 메모리 사용량 최소화를 위한 공유 스택 모드 지원
* 동기화 프리미티브
  + 코루틴 뮤텍스, 셈포어는 코루틴 간에 사용 가능
  + 코루틴과 스레드 간에 코루틴 이벤트 사용 가능
 
<br>

--------------------

## 예제
- [CoroutineTCP 소스코드](../ServerAcl/tcpCorroutine.cpp)
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




