#include "pch.h"
#include <signal.h>
#include <print>

// ���� �ڿ�
int shared_resource_for_fcond = 0;
acl::fiber_mutex mutex_cond;
acl::fiber_cond cond;  // fiber_cond ���

// ������ ��ٸ��� �ڷ�ƾ
void condition_waiting_fiber(int fiber_id) {
    std::print("Fiber {} is waiting for condition...\n", fiber_id);

    mutex_cond.lock();
    cond.wait(mutex_cond);  // ���� ������ ��ٸ�

    std::print("Fiber {} condition met, shared resource: \n", fiber_id, shared_resource_for_fcond);

    mutex_cond.unlock();
}

// ������ ������Ű�� �ڷ�ƾ
void condition_notifier_fiber(int fiber_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // ��ٸ�
    std::print("Fiber {} is signaling the condition...\n", fiber_id);

    mutex_cond.lock();
    shared_resource_for_fcond++;
    cond.notify();  // ���� ���� �ñ׳� ����
    mutex_cond.unlock();
}

int main_fcond() {
    // ACL �ʱ�ȭ
    acl::acl_cpp_init();

    // ������ ��ٸ��� �ڷ�ƾ 1��
    go[&]() {
        condition_waiting_fiber(1);
    };

    // ������ ������Ű�� �ڷ�ƾ 1��
    go[&]() {
        condition_notifier_fiber(2);
    };

    // �ڷ�ƾ �����ٷ� ����
    acl::fiber::schedule();

    std::print("Final shared resource value: {}\n", shared_resource_for_fcond);

    return 0;
}
