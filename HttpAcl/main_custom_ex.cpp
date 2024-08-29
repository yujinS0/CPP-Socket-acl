#include "acl_cpp/lib_acl.hpp"
#include "fiber/http_server.hpp"

int main_sample() {
    acl::acl_cpp_init(); // ACL ���̺귯�� �ʱ�ȭ
    const char* addr = "0.0.0.0:8080"; // ���� �ּҿ� ��Ʈ ����

    // HTTP ���� ��ü ����
    acl::http_server server;

    // GET /
    server.Get("/", [](acl::HttpRequest&, acl::HttpResponse& res) {
        std::string response = "Hello, World!";
        res.setContentType("text/plain");
        res.setContentLength(response.size());
        return res.write(response.c_str(), response.size());
        });

    // POST /data
    server.Post("/data", [](acl::HttpRequest& req, acl::HttpResponse& res) {
        acl::string* body = req.getBody();
        std::string response = "Received: ";
        if (body != nullptr) {
            response += body->c_str(); // acl::string�� const char*�� ��ȯ�Ͽ� ���
        }
        else {
            response += "(no data)";
        }
        res.setContentType("text/plain");
        res.setContentLength(response.size());
        return res.write(response.c_str(), response.size());
        });

    // POST /json
    server.Post("/json", [](acl::HttpRequest& req, acl::HttpResponse& res) {
        acl::string* body = req.getBody();
        std::string body_content = body ? body->c_str() : "(empty)";
        std::cout << "Request Body: " << body_content << std::endl;

        // ���� JSON �Ľ� �õ�
        acl::json json;
        if (json.update(body_content.c_str())) { // JSON�� �������� �Ľ�
            std::cout << "Parsed JSON: " << json.to_string() << std::endl;
            return res.write(json); // JSON ���� ����
        }
        else {
            std::string buf = "failed to parse json\r\n";
            std::cout << "Failed to parse JSON" << std::endl;
            res.setContentLength(buf.size());
            return res.write(buf.c_str(), buf.size());
        }
        });


    // ���� ����
    server.run_alone(addr);

    return 0;
}
