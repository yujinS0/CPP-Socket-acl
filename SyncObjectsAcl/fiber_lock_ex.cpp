#include "pch.h"
#include <random>
#include <print>

// ���� �ڿ�
int shared_resource_flock = 0;
acl::fiber_lock lock;  // fiber_lock ���

// �ڿ��� �����ϴ� �ڷ�ƾ
void lock_fiber(int fiber_id) {
    for (int i = 0; i < 10; ++i) {
        lock.lock();  // fiber_lock Ȱ��ȭ
        std::print("Fiber {} locked the resource at iteration {}\n", fiber_id, i);

        shared_resource_flock++;
        std::print("Fiber {} accessed resource: {}\n", fiber_id, shared_resource_flock);

        lock.unlock();  // fiber_lock ����
        std::print("Fiber {} unlocked the resource at iteration {}\n", fiber_id, i);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


// �� �����忡�� �ڷ�ƾ�� �����ϴ� �Լ�
static void thread_func(int thread_id) {
    for (int i = 1; i <= 100; ++i) { 
        go[i]() {
            lock_fiber(i);
        };
    }

    acl::fiber::schedule();
}

int main_flock() {
    // ACL �ʱ�ȭ
    acl::acl_cpp_init();

    std::vector<std::thread> threads; // ���� ������� ����!
    for (int i = 0; i < 1; ++i) { 
        threads.emplace_back(thread_func, i);  // �� �����忡 �ڷ�ƾ ���� �Լ� �Ҵ�
    }

    // ��� �����尡 ����� ������ ���
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();  // ������ ���� ���
        }
    }
    std::print("Final shared resource value: {}\n", shared_resource_flock); // ���� 50

    return 0;
}
