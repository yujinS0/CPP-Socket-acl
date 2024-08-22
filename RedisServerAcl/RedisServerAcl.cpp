#include "pch.h"
#include "redisHandler.h"

#include <acl_cpp/lib_acl.hpp>

// 구조체 정의
struct User {
    std::string name;
    int age;
    std::string email;
};

// 구조체를 ACL JSON 객체로 변환하는 함수
void user_to_json(const User& user, acl::json& json) {
    acl::json_node& root = json.get_root();
    root.add_text("name", user.name.c_str());
    root.add_number("age", user.age);
    root.add_text("email", user.email.c_str());
}

//User json_to_user(acl::json& json) {
//    User user;
//
//    // JSON이 배열인 경우 첫 번째 요소를 가져옴
//    acl::json_node* root = json.get_root().at(0);
//
//    if (!root) {
//        std::cerr << "Failed to get the first element of the JSON array!" << std::endl;
//        return user;
//    }
//
//    // 각 필드를 가져옴
//    acl::json_node* name_node = (*root)["name"];
//    acl::json_node* age_node = (*root)["age"];
//    acl::json_node* email_node = (*root)["email"];
//
//    // 값이 존재하는지 확인한 후 가져옵니다.
//    if (name_node && name_node->is_string()) {
//        user.name = name_node->get_text();
//        std::cout << "Name: " << user.name << std::endl;
//    }
//    else {
//        std::cerr << "Name node missing or not a string!" << std::endl;
//    }
//
//    if (age_node && age_node->is_number()) {
//        user.age = static_cast<int>(age_node->get_int64() ? *age_node->get_int64() : 0);
//        std::cout << "Age: " << user.age << std::endl;
//    }
//    else {
//        std::cerr << "Age node missing or not a number!" << std::endl;
//    }
//
//    if (email_node && email_node->is_string()) {
//        user.email = email_node->get_text();
//        std::cout << "Email: " << user.email << std::endl;
//    }
//    else {
//        std::cerr << "Email node missing or not a string!" << std::endl;
//    }
//
//    return user;
//}


// JSON을 User 구조체로 변환 (디버깅용 로그 추가)
User json_to_user(acl::json& json) {
    acl::json_node& root = json.get_root();
    User user;

    // 노드의 첫 번째 자식 노드를 얻습니다.
    acl::json_node* name_node = root["name"];
    acl::json_node* age_node = root["age"];
    acl::json_node* email_node = root["email"];

    // 값이 존재하는지 확인한 후 가져옵니다.
    if (name_node && name_node->is_string()) {
        user.name = name_node->get_text();
        std::cout << "Name: " << user.name << std::endl;
    }
    else {
        std::cerr << "Name node missing or not a string!" << std::endl;
    }

    if (age_node && age_node->is_number()) {
        user.age = static_cast<int>(age_node->get_int64() ? *age_node->get_int64() : 0);
        std::cout << "Age: " << user.age << std::endl;
    }
    else {
        std::cerr << "Age node missing or not a number!" << std::endl;
    }

    if (email_node && email_node->is_string()) {
        user.email = email_node->get_text();
        std::cout << "Email: " << user.email << std::endl;
    }
    else {
        std::cerr << "Email node missing or not a string!" << std::endl;
    }

    return user;
}


//User json_to_user(acl::json& json) {
//    acl::json_node& root = json.get_root();
//    User user;
//
//    // 노드의 첫 번째 자식 노드를 얻습니다.
//    acl::json_node* name_node = root["name"];
//    acl::json_node* age_node = root["age"];
//    acl::json_node* email_node = root["email"];
//
//    // 값이 존재하는지 확인한 후 가져옵니다.
//    if (name_node && name_node->is_string()) {
//        user.name = name_node->get_text();
//    }
//
//    if (age_node && age_node->is_number()) {
//        user.age = static_cast<int>(age_node->get_int64() ? *age_node->get_int64() : 0);
//    }
//
//    if (email_node && email_node->is_string()) {
//        user.email = email_node->get_text();
//    }
//
//    return user;
//}

// Redis에 JSON 데이터를 저장하는 함수 (JSON.SET 명령 사용)
bool set_json_data(acl::redis_client& client, const std::string& key, const User& user) {
    // User 구조체를 JSON으로 변환
    acl::json json;
    user_to_json(user, json);

    // JSON 데이터를 문자열로 변환
    acl::string json_str;
    json.build_json(json_str);

    // Redis 명령을 설정
    acl::redis_command cmd(&client);
    std::vector<const char*> argv = { "JSON.SET", key.c_str(), "$", json_str.c_str() };
    std::vector<size_t> lens = { strlen("JSON.SET"), key.size(), strlen("$"), json_str.size() };

    // Redis 명령 실행
    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());

    // 명령 실행 결과 확인
    return result && result->get_type() == acl::REDIS_RESULT_STATUS;
}

// Redis에서 JSON 데이터를 가져오는 함수 (JSON.GET 명령 사용)
User get_json_data(acl::redis_client& client, const std::string& key) {
    // Redis 명령을 설정
    acl::redis_command cmd(&client);
    std::vector<const char*> argv = { "JSON.GET", key.c_str(), "$" };
    std::vector<size_t> lens = { strlen("JSON.GET"), key.size(), strlen("$") };

    // 명령 실행
    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());

    // 결과 확인
    if (!result || result->get_type() != acl::REDIS_RESULT_STRING) {
        throw std::runtime_error("Failed to retrieve JSON data from Redis");
    }

    // 결과 출력 (디버깅용)
    std::cout << "Raw JSON data from Redis: " << result->get(0) << std::endl;

    // 가져온 JSON 데이터를 파싱
    acl::json json;
    json.update(result->get(0));

    // JSON 데이터 출력 (디버깅용)
    std::cout << "Parsed JSON data: " << json.to_string() << std::endl;

    // JSON 데이터를 User 구조체로 변환
    return json_to_user(json);
}

// 특정 필드만 Redis에서 가져오는 함수 (JSON.GET 명령 사용)
std::string get_json_field(acl::redis_client& client, const std::string& key, const std::string& field) {
    // Redis 명령을 설정
    acl::redis_command cmd(&client);
    std::vector<const char*> argv = { "JSON.GET", key.c_str(), field.c_str() };
    std::vector<size_t> lens = { strlen("JSON.GET"), key.size(), field.size() };

    // 명령 실행
    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());

    // 결과 확인
    if (!result || result->get_type() != acl::REDIS_RESULT_STRING) {
        throw std::runtime_error("Failed to retrieve JSON field from Redis");
    }

    // 결과 문자열을 반환
    return result->get(0);
}

//// Redis에서 JSON 데이터를 가져오는 함수 (JSON.GET 명령 사용)
//User get_json_data(acl::redis_client& client, const std::string& key) {
//    // Redis 명령을 설정
//    acl::redis_command cmd(&client);
//    std::vector<const char*> argv = { "JSON.GET", key.c_str(), "$" };
//    std::vector<size_t> lens = { strlen("JSON.GET"), key.size(), strlen("$") };
//
//    // 명령 실행
//    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), lens.data());
//
//    // 결과 확인
//    if (!result || result->get_type() != acl::REDIS_RESULT_STRING) {
//        throw std::runtime_error("Failed to retrieve JSON data from Redis");
//    }
//
//    // 가져온 JSON 데이터를 파싱
//    acl::json json;
//    json.update(result->get(0));
//
//    // JSON 데이터를 User 구조체로 변환
//    return json_to_user(json);
//}


//////----- 예외 발생하는 코드 
//// Redis에 JSON 데이터를 저장하는 함수 (JSON.SET 명령 사용)
//bool set_json_data(acl::redis_client& client, const std::string& key, const User& user) {
//    // User 구조체를 JSON으로 변환
//    acl::json json;
//    user_to_json(user, json);
//
//    // JSON 데이터를 문자열로 변환
//    acl::string json_str;
//    json.build_json(json_str);
//
//    // Redis 명령을 설정
//    acl::redis_command cmd(&client);
//    std::vector<const char*> argv = { "JSON.SET", key.c_str(), "$", json_str.c_str() };
//
//    // Redis 명령 실행
//    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), nullptr);
//
//    // 명령 실행 결과 확인
//    return result && result->get_type() == acl::REDIS_RESULT_STATUS;
//}
//
//// Redis에서 JSON 데이터를 가져오는 함수 (JSON.GET 명령 사용)
//User get_json_data(acl::redis_client& client, const std::string& key) {
//    // Redis 명령을 설정
//    acl::redis_command cmd(&client);
//    std::vector<const char*> argv = { "JSON.GET", key.c_str(), "$" };
//
//    // 명령 실행
//    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), nullptr);
//
//    // 결과 확인
//    if (!result || result->get_type() != acl::REDIS_RESULT_STRING) {
//        throw std::runtime_error("Failed to retrieve JSON data from Redis");
//    }
//
//    // 가져온 JSON 데이터를 파싱
//    acl::json json;
//    json.update(result->get(0));
//
//    // JSON 데이터를 User 구조체로 변환
//    return json_to_user(json);
//}
//----------------------

int main() {
    SetConsoleOutputCP(CP_UTF8); // 콘솔의 코드 페이지를 UTF-8로 설정

    // ACL 라이브러리 초기화
    acl::acl_cpp_init();
    acl::log::stdout_open(true);

    // Redis 클라이언트 설정
    const char* redis_addr = "127.0.0.1:6389";
    acl::redis_client client(redis_addr, 10, 10);

    // User 구조체 생성
    User user = { "John Doe", 30, "john.doe@example.com" };

    // Redis에 JSON 데이터 저장
    if (set_json_data(client, "user:1", user)) {
        std::cout << "User data saved successfully in Redis!" << std::endl;
    }
    else {
        std::cerr << "Failed to save user data in Redis." << std::endl;
    }

    // 특정 필드 가져오기 - 예시
    try {
        std::string name = get_json_field(client, "user:1", "name");
        std::cout << "User's name: " << name << std::endl;

        std::string age = get_json_field(client, "user:1", "age");
        std::cout << "User's age: " << age << std::endl;

        std::string email = get_json_field(client, "user:1", "email");
        std::cout << "User's email: " << email << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

//// Redis에 JSON 데이터를 저장하는 함수
//bool set_json_data(acl::redis& redis, const std::string& key, const User& user) {
//    // User 구조체를 JSON으로 변환
//    acl::json json;
//    user_to_json(user, json);
//
//    // JSON 데이터를 문자열로 변환
//    acl::string json_str;
//    json.build_json(json_str);
//
//    // Redis에 JSON 데이터를 저장 (JSON.SET 명령 사용)
//    std::vector<const char*> argv = { "JSON.SET", key.c_str(), "$", json_str.c_str() };
//
//    // Redis 명령 실행
//    const acl::redis_result* result = redis.exec(argv.size(), argv.data());
//
//    // 명령 실행 결과 확인
//    return result && result->get_type() == acl::REDIS_RESULT_STATUS;
//}

//// Redis에서 JSON 데이터를 가져오는 함수
//User get_json_data(acl::redis& redis, const std::string& key) {
//    // Redis에서 JSON 데이터를 가져옴 (JSON.GET 명령 사용)
//    std::vector<const char*> argv = { "JSON.GET", key.c_str(), "$" };
//
//    // 명령 실행
//    const acl::redis_result* result = redis.exec(argv.size(), argv.data());
//
//    // 결과 확인
//    if (!result || result->get_type() != acl::REDIS_RESULT_STRING) {
//        throw std::runtime_error("Failed to retrieve JSON data from Redis");
//    }
//
//    // 가져온 JSON 데이터를 파싱
//    acl::json json;
//    json.update(result->get(0));
//
//    // JSON 데이터를 User 구조체로 변환
//    return json_to_user(json);
//}


////////////////////////////////
//// Redis에 JSON 데이터를 저장하는 함수
//bool set_json_data(acl::redis_client& client, const std::string& key, const User& user) {
//    // User 구조체를 JSON으로 변환
//    acl::json json;
//    user_to_json(user, json);
//
//    // JSON 데이터를 문자열로 변환
//    acl::string json_str;
//    json.build_json(json_str);
//
//    // Redis 명령어 설정
//    acl::redis_command cmd(&client);
//    std::vector<const char*> argv = { "JSON.SET", key.c_str(), "$", json_str.c_str() };
//
//    // 명령 실행
//    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), nullptr);
//
//    // 명령 실행 결과 확인
//    return result && result->get_type() == REDIS_RESULT_STATUS;
//}
//
//// Redis에서 JSON 데이터를 가져오는 함수
//User get_json_data(acl::redis_client& client, const std::string& key) {
//    // Redis 명령어 설정
//    acl::redis_command cmd(&client);
//    std::vector<const char*> argv = { "JSON.GET", key.c_str(), "$" };
//
//    // 명령 실행
//    const acl::redis_result* result = cmd.request(argv.size(), argv.data(), nullptr);
//
//    // 결과 확인
//    if (!result || result->get_type() != REDIS_RESULT_STRING) {
//        throw std::runtime_error("Failed to retrieve JSON data from Redis");
//    }
//
//    // 가져온 JSON 데이터를 파싱
//    acl::json json;
//    json.update(result->get(0));
//
//    // JSON 데이터를 User 구조체로 변환
//    return json_to_user(json);
//}


//int main() {
//    // ACL 라이브러리 초기화
//    acl::acl_cpp_init();
//    acl::log::stdout_open(true);
//
//    // Redis 클라이언트 설정
//    const char* redis_addr = "127.0.0.1:6379";
//    acl::redis_client client(redis_addr, 10, 10);
//    acl::redis redis;
//    redis.set_client(&client);
//
//    // User 구조체 생성
//    User user = { "John Doe", 30, "john.doe@example.com" };
//
//    // Redis에 JSON 데이터 저장
//    if (set_json_data(redis, "user:1", user)) {
//        std::cout << "User data saved successfully in Redis!" << std::endl;
//    }
//    else {
//        std::cerr << "Failed to save user data in Redis." << std::endl;
//    }
//
//    // Redis에서 JSON 데이터 가져오기
//    try {
//        User retrieved_user = get_json_data(redis, "user:1");
//        std::cout << "Retrieved User - Name: " << retrieved_user.name
//            << ", Age: " << retrieved_user.age
//            << ", Email: " << retrieved_user.email << std::endl;
//    }
//    catch (const std::exception& e) {
//        std::cerr << e.what() << std::endl;
//    }
//
//    return 0;
//}

///////////////////////////////////////
//int main() {
//    acl::acl_cpp_init();
//    acl::log::stdout_open(true);
//
//    acl::redis_client* client = setup_redis();
//    if (!client) {
//        return -1;
//    }
//
//    acl::redis redis;
//    redis.set_client(client);
//
//    while (true) {
//        std::string command;
//        std::cout << "\n[Redis Test with Acl] 명령어를 입력하세요 (set, get, list_push, list_get, hash, zset, exit): ";
//        std::cin >> command;
//
//        if (command == "exit") {
//            break;
//        }
//        else if (command == "set") {
//            handle_string(redis);
//        }
//        else if (command == "get") {
//            handle_get(redis);
//        }
//        else if (command == "list_push") {
//            handle_list_push(redis);
//        }
//        else if (command == "list_get") {
//            handle_list_get(redis);
//        }
//        else if (command == "hash") {
//            handle_hash(redis);
//        }
//        else if (command == "zset") {
//            handle_zset(redis);
//        }
//        else {
//            std::cout << "지원하지 않는 명령어입니다." << std::endl;
//        }
//    }
//
//    delete client;
//    return 0;
//}