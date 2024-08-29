#pragma once
#include <string>
#include "acl_cpp/lib_acl.hpp"
#include "http_model.hpp"

class HttpService {
public:
    std::string process_root_request() {
        // 이 로직은 복잡해질 수 있으며, 필요한 경우 Model을 호출할 수 있습니다.
        return "hello world1!\r\n";
    }

    std::string process_json_request(acl::json* json) {
        if (json) {
            // 필요한 비즈니스 로직 수행
            return json->to_string();
        }
        else {
            return "no json got\r\n";
        }
    }
};
