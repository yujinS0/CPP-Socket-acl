#include "pch.h"

// ���� �ڿ�
int shared_resource_for_fevent = 0;
acl::fiber_mutex mutex;
acl::fiber_event event;  // fiber_event ���

// �̺�Ʈ�� ��ٸ��� �ڷ�ƾ
void event_waiting_fiber(int fiber_id) {
    std::cout << "Fiber " << fiber_id << " is waiting for an event...\n";
    event.wait();  // �̺�Ʈ �߻��� ��ٸ�

    mutex.lock();
    shared_resource_for_fevent++;
    std::cout << "Fiber " << fiber_id << " processed event, shared resource: " << shared_resource_for_fevent << std::endl;
    mutex.unlock();
}

// �̺�Ʈ�� �߻���Ű�� �ڷ�ƾ
void event_notifier_fiber(int fiber_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // ��ٸ�
    std::cout << "Fiber " << fiber_id << " is notifying the event...\n";
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

    std::cout << "Final shared resource value: " << shared_resource_for_fevent << std::endl;

    return 0;
}
