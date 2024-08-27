#include "pch.h"

// 공유 자원
int shared_resource_for_fevent = 0;
acl::fiber_mutex mutex;
acl::fiber_event event;  // fiber_event 사용

// 이벤트를 기다리는 코루틴
void event_waiting_fiber(int fiber_id) {
    std::cout << "Fiber " << fiber_id << " is waiting for an event...\n";
    event.wait();  // 이벤트 발생을 기다림

    mutex.lock();
    shared_resource_for_fevent++;
    std::cout << "Fiber " << fiber_id << " processed event, shared resource: " << shared_resource_for_fevent << std::endl;
    mutex.unlock();
}

// 이벤트를 발생시키는 코루틴
void event_notifier_fiber(int fiber_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 기다림
    std::cout << "Fiber " << fiber_id << " is notifying the event...\n";
    event.notify();  // 이벤트 발생
}

int main_fevent() {
    // ACL 초기화
    acl::acl_cpp_init();

    // 이벤트를 기다리는 코루틴 1개
    go[&]() {
        event_waiting_fiber(1);
    };

    // 이벤트를 발생시키는 코루틴 1개
    go[&]() {
        event_notifier_fiber(2);
    };

    // 코루틴 스케줄러 실행
    acl::fiber::schedule();

    std::cout << "Final shared resource value: " << shared_resource_for_fevent << std::endl;

    return 0;
}
