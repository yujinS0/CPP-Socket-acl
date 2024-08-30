#include "pch.h"
#include <print>

// �б�-���� �� ��ü
acl::fiber_rwlock rwlock;
int shared_resource_rwlock = 0;

void reader_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        rwlock.rlock();  // �б� ��
        std::print("Reader Fiber {} read the resource: {} at iteration {}\n", fiber_id, shared_resource_rwlock, i);
        rwlock.runlock(); // �б� �� ����
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void writer_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        rwlock.wlock();  // ���� ��
        shared_resource_rwlock++;
        std::print("Writer Fiber {} updated resource to: {} at iteration {}\n", fiber_id, shared_resource_rwlock, i);
        rwlock.wunlock(); // ���� �� ����
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main_rw() {
    acl::acl_cpp_init();

    // 2���� �б� �ڷ�ƾ ����
    go[&]() {
        reader_fiber(1);
    };
    go[&]() {
        reader_fiber(2);
    };

    // 1���� ���� �ڷ�ƾ ����
    go[&]() {
        writer_fiber(3);
    };

    acl::fiber::schedule();
    std::print("Final shared resource value: {}\n", shared_resource_rwlock);
    return 0;
}
