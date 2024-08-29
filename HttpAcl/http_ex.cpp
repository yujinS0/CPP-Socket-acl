#include "acl_cpp/lib_acl.hpp"  // Must before http_server.hpp
#include "fiber/http_server.hpp"

// 서버의 설정 변수
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


int main_http_test() {
    const char* addr = "0.0.0.0|8194";

    acl::acl_cpp_init();

    acl::http_server server;

    server.set_cfg_int(var_conf_int_tab).set_cfg_str(var_conf_str_tab); // 위에서 정한 서버 설정값을 기반으로 초기화

    // Call the methods in acl::http_server.
    server.on_proc_init([&addr] { // 서버 프로세스 초기화된 후 실행되는 코드
        printf("---> after process init: addr=%s, io_timeout=%d\r\n", addr, var_cfg_io_timeout);
        }).on_proc_exit([] { // 서버 프로세스 종료되기 전 실행되는 코드
            printf("---> before process exit\r\n");
            }).on_proc_sighup([](acl::string& s) { // 서버가 SIGHUP 시그널(서버 설정 재로드 or 재시작) 수신 시 실행될 코드
                s = "+ok";
                printf("---> process got sighup\r\n");
                return true;
                }).on_thread_accept([](acl::socket_stream& conn) { // 클라이언트 연결 수락 시 실행될 코드
                    printf("---> accept %d\r\n", conn.sock_handle());
                    return true;
                    }).Get("/", [](acl::HttpRequest&, acl::HttpResponse& res) {
                        std::string buf("hello world1!\r\n");
                        res.setContentLength(buf.size());
                        return res.write(buf.c_str(), buf.size());
                        }).Post("/json", [](acl::HttpRequest& req, acl::HttpResponse& res) {
                            acl::json* json = req.getJson();
                            if (json) {
                                return res.write(*json);
                            }
                            else {
                                std::string buf = "no json got\r\n";
                                res.setContentLength(buf.size());
                                return res.write(buf.c_str(), buf.size());
                            }
                            }).run_alone(addr); // 서버 alone 모드로 실행

                        return 0;
}
