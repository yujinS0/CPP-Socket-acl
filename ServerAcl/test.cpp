//#include "pch.h"
//
//---[Test First Demo]---
//int main(void) {
//	acl::string buf = "hello world!\r\n";
//	std::cout << buf.c_str() << std::endl;
//	return 0;
//}

//---[Test Coroutine]---
//class fiber_client : public acl::fiber {
//public:
//    fiber_client(acl::socket_stream* conn) : conn_(conn) {}
//
//private:
//    ~fiber_client(void) { delete conn_; }
//
//private:
//    acl::socket_stream* conn_;
//
//    // @override
//    void run(void) {
//        char buf[256];
//        while (true) {
//            int ret = conn_->read(buf, sizeof(buf), false);
//            if (ret <= 0) {
//                break;
//            }
//            if (conn_->write(buf, ret) != ret) {
//                break;
//            }
//        }
//
//        delete this;
//    }
//};
//
//class fiber_server : public acl::fiber {
//public:
//    fiber_server(acl::server_socket& server) : server_(server) {}
//
//private:
//    ~fiber_server(void) {}
//
//private:
//    acl::server_socket& server_;
//
//    // @override
//    void run(void) {
//        while (true) {
//            acl::socket_stream* conn = server_.accept();
//            if (conn) {
//                acl::fiber* fb = new fiber_client(conn);
//                fb->start();
//            }
//        }
//
//        delete this;
//    }
//};
//
//int main(void) {
//    const char* addr = "127.0.0.1:8088";
//    acl::server_socket server;
//    if (!server.open(addr)) {
//        return 1;
//    }
//
//    // Create one server coroutine to wait for connection.
//    acl::fiber* fb = new fiber_server(server);
//    fb->start();
//
//    acl::fiber::schedule();  // Start the coroutine scheculde process.
//    return 0;
//}