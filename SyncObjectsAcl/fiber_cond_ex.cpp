#include "pch.h"
#include <signal.h>

// 공유 자원
int shared_resource_for_fcond = 0;
acl::fiber_mutex mutex_cond;
acl::fiber_cond cond;  // fiber_cond 사용

// 조건을 기다리는 코루틴
void condition_waiting_fiber(int fiber_id) {
    std::cout << "Fiber " << fiber_id << " is waiting for condition...\n";

    mutex_cond.lock();
    cond.wait(mutex_cond);  // 조건 충족을 기다림
    std::cout << "Fiber " << fiber_id << " condition met, shared resource: " << shared_resource_for_fcond << std::endl;
    mutex_cond.unlock();
}

// 조건을 만족시키는 코루틴
void condition_notifier_fiber(int fiber_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 기다림
    std::cout << "Fiber " << fiber_id << " is signaling the condition...\n";

    mutex_cond.lock();
    shared_resource_for_fcond++;
    cond.notify();  // 조건 충족 시그널 전송
    mutex_cond.unlock();
}

int main_fcond() {
    // ACL 초기화
    acl::acl_cpp_init();

    // 조건을 기다리는 코루틴 1개
    go[&]() {
        condition_waiting_fiber(1);
    };

    // 조건을 만족시키는 코루틴 1개
    go[&]() {
        condition_notifier_fiber(2);
    };

    // 코루틴 스케줄러 실행
    acl::fiber::schedule();

    std::cout << "Final shared resource value: " << shared_resource_for_fcond << std::endl;

    return 0;
}
