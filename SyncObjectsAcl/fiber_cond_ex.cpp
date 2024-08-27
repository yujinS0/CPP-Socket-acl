#include "pch.h"
#include <signal.h>

// ���� �ڿ�
int shared_resource_for_fcond = 0;
acl::fiber_mutex mutex_cond;
acl::fiber_cond cond;  // fiber_cond ���

// ������ ��ٸ��� �ڷ�ƾ
void condition_waiting_fiber(int fiber_id) {
    std::cout << "Fiber " << fiber_id << " is waiting for condition...\n";

    mutex_cond.lock();
    cond.wait(mutex_cond);  // ���� ������ ��ٸ�
    std::cout << "Fiber " << fiber_id << " condition met, shared resource: " << shared_resource_for_fcond << std::endl;
    mutex_cond.unlock();
}

// ������ ������Ű�� �ڷ�ƾ
void condition_notifier_fiber(int fiber_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // ��ٸ�
    std::cout << "Fiber " << fiber_id << " is signaling the condition...\n";

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

    std::cout << "Final shared resource value: " << shared_resource_for_fcond << std::endl;

    return 0;
}
