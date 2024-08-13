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

void handle_client(acl::socket_stream* conn) {
    char buf[256];
    while (true) {
        int ret = conn->read(buf, sizeof(buf), false);  // 클라이언트로부터 데이터 읽기
        if (ret <= 0) break;  // 연결이 종료되거나 오류 발생 시 루프 탈출

        conn->write(buf, ret);  // 읽은 데이터를 클라이언트로 에코
    }
    delete conn;  // 연결 종료 후 소켓 삭제
}

int main() {
    const char* addr = "127.0.0.1:8088";
    acl::acl_cpp_init();  // ACL 초기화
    acl::server_socket server;

    if (!server.open(addr)) {  // 로컬 주소에 바인드하고 리슨
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return -1;
    }

    std::cout << "Server is running on " << addr << std::endl;

    while (true) {
        acl::socket_stream* conn = server.accept(); // 클라이언트 연결 대기
        if (conn == nullptr) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        std::thread client_thread(handle_client, conn);  // 새 스레드에서 클라이언트 처리
        client_thread.detach();  // 스레드를 분리하여 독립적으로 실행되도록 함
    }

    return 0;
}