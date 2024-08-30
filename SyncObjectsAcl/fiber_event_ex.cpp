#include "pch.h"
#include <print>

// ���� �ڿ�
int shared_resource_for_fevent = 0;
acl::fiber_mutex mutex;
acl::fiber_event event;  // fiber_event ���

// �̺�Ʈ�� ��ٸ��� �ڷ�ƾ
void event_waiting_fiber(int fiber_id) {
    std::print("Fiber {} is waiting for an event...\n", fiber_id);
    event.wait();  // �̺�Ʈ �߻��� ��ٸ�

    mutex.lock();
    shared_resource_for_fevent++;
    std::print("Fiber {} processed event, shared resource: \n", fiber_id, shared_resource_for_fevent); 
    
    mutex.unlock();
}

// �̺�Ʈ�� �߻���Ű�� �ڷ�ƾ
void event_notifier_fiber(int fiber_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // ��ٸ�
    std::print("Fiber {} is notifying the event...\n", fiber_id);
    event.notify();  // �̺�Ʈ �߻�
}

int main_fevent() {
    // ACL �ʱ�ȭ
    acl::acl_cpp_init();

    // �̺�Ʈ�� ��ٸ��� �ڷ�ƾ 1��
    go[&]() {
        event_waiting_fiber(1);
    };

    // �̺�Ʈ�� �߻���Ű�� �ڷ�ƾ 1��
    go[&]() {
        event_notifier_fiber(2);
    };

    // �ڷ�ƾ �����ٷ� ����
    acl::fiber::schedule();

    std::print("Final shared resource value: {}\n", shared_resource_for_fevent);

    return 0;
}
