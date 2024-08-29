#pragma once
#include <string>
#include "acl_cpp/lib_acl.hpp"
#include "http_model.hpp"

class HttpService {
public:
    std::string process_root_request() {
        // �� ������ �������� �� ������, �ʿ��� ��� Model�� ȣ���� �� �ֽ��ϴ�.
        return "hello world1!\r\n";
    }

    std::string process_json_request(acl::json* json) {
        if (json) {
            // �ʿ��� ����Ͻ� ���� ����
            return json->to_string();
        }
        else {
            return "no json got\r\n";
        }
    }
};
