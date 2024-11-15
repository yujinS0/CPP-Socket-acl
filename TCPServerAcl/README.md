### TCP Echo Server with Acl

**TCPServerAcl** 프로젝트는 ACL(Advanced C/C++ Library) 프레임워크를 사용하여 구현한 간단한 TCP 에코 서버입니다. <br>
이 서버는 클라이언트로부터 수신한 데이터를 그대로 다시 클라이언트에게 반환하는 기능을 제공합니다. <br>
이 서버의 클라이언트는 [📁CppConsoleClient 디렉토리](../CppConsoleClient)에 존재합니다.

---

## 프로젝트 구조

```
TCPServerAcl/
├── TCPServerAcl.cpp  # 프로젝트의 메인 파일로, 서버 초기화 및 실행을 담당
├── tcp.cpp              # TCP 서버의 핵심 기능을 구현한 파일
├── tcp.h                # TCP 서버 함수들의 선언을 포함한 헤더 파일
├── pch.h                # 프로젝트 전반에서 사용되는 공통 헤더 파일
```

---

## 주요 기능

### 1. 서버 초기화 및 실행 (`TCPServerAcl.cpp`)
- **메인 엔트리 포인트**: `main()` 함수는 프로그램의 시작점으로, ACL 라이브러리 초기화와 TCP 서버 실행을 담당합니다.
- **예외 처리**: 서버 실행 중 발생할 수 있는 예외를 처리하여 안정성을 보장합니다.

### 2. 클라이언트 처리 (`tcp.cpp`)
- **핸들링 클라이언트**: `handle_client()` 함수는 클라이언트로부터 데이터를 수신하고, 이를 다시 클라이언트에게 반환하는 에코 기능을 수행합니다.
- **서버 실행**: `run_tcp_server()` 함수는 서버 소켓을 초기화하고, 클라이언트 연결을 대기하며, 각 연결을 새로운 스레드에서 처리합니다.

### 3. 공통 헤더 파일 (`pch.h`)
- **프리컴파일 헤더**: 자주 사용되는 표준 라이브러리와 ACL 라이브러리의 헤더를 포함하여 컴파일 시간을 단축합니다.

---

## 빌드 및 실행

1. **빌드**:
   - Visual Studio에서 솔루션 파일(`TCPServerAcl.sln`)을 열고, `빌드` 메뉴에서 프로젝트를 빌드합니다.

2. **실행**:
   - 빌드가 완료되면, 실행 파일을 통해 서버를 시작할 수 있습니다. 서버는 기본적으로 `127.0.0.1:8088`에서 클라이언트의 연결을 대기합니다.

3. **클라이언트 연결**:
   - 클라이언트 프로그램을 통해 서버에 연결하고, 데이터를 전송하면 서버는 해당 데이터를 그대로 반환합니다.

---

## 예제 코드

**메인 엔트리 포인트:**

```cpp
int main() {
    try {
        SetConsoleOutputCP(CP_UTF8); // 콘솔의 코드 페이지를 UTF-8로 설정
        acl::acl_cpp_init();  // ACL 초기화
        run_tcp_server();  // TCP 서버 실행 함수 호출
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
    return 0;
}
```

**TCP 서버 실행:**

```cpp
void run_tcp_server() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;

    if (!server.open(addr)) {
        std::print("Failed to open server socket on {}\n", addr);
        return;
    }

    std::print("Server is running on {}\n", addr);

    while (true) {
        acl::socket_stream* conn = server.accept(); // 클라이언트 연결 대기
        if (conn == nullptr) {
            std::print("Failed to accept connection\n");
            continue;
        }

        std::thread client_thread(handle_client, conn);  // 새 스레드에서 클라이언트 처리
        client_thread.detach();  // 스레드를 분리하여 독립적으로 실행되도록 함
    }
}
```

---

## 결론

TCPServerAcl 프로젝트는 간단한 TCP 에코 서버를 구현한 예제로, ACL 라이브러리와 C++의 멀티스레딩 기능을 활용합니다. 이 프로젝트를 통해 네트워크 프로그래밍의 기본적인 개념을 익히고, ACL 라이브러리의 사용 방법을 학습할 수 있습니다.
