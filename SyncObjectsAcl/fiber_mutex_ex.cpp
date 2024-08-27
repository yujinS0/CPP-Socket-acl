#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"

// 공유 자원
int shared_resource = 0;
acl::fiber_mutex mutex;

// 코루틴 함수
void test_fiber_mutex(int fiber_id) {
    for (int i = 0; i < 10; ++i) {
        mutex.lock(); // mutex lock 주석 시 -> 최종 값이 50
        std::cout << "Mutex locked by [fiber ID: " << fiber_id << "] at iteration " << i << std::endl;

        // 공유 자원에 접근
        int temp = shared_resource;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 경합 유발
        shared_resource = temp + 1;

        std::cout << "Fiber ID " << fiber_id << " accessed resource: " << shared_resource << " at iteration " << i << std::endl;

        mutex.unlock(); // mutex lock 주석 시 -> 최종 값이 50
        std::cout << "Mutex unlocked by [fiber ID: " << fiber_id << "] at iteration " << i << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    }
}

// 각 스레드에서 코루틴을 실행하는 함수
void thread_func(int thread_id) {
    for (int i = 1; i <= 5; ++i) {   // 5개의 코루틴 생성
        go[i]() {
            test_fiber_mutex(i);
        };
    }

    acl::fiber::schedule();
}

int main() {
    // ACL 초기화
    acl::acl_cpp_init();

    // 5개의 스레드 생성
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(thread_func, i);  // 각 스레드에 코루틴 실행 함수 할당
    }

    // 모든 스레드가 종료될 때까지 대기
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();  // 스레드 종료 대기
        }
    }

    std::cout << "Final shared resource value: " << shared_resource << std::endl; // 정상적인 경우 250

    return 0;
}