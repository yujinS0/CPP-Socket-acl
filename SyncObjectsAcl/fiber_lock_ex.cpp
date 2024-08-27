#include "pch.h"
#include <random>

// ���� �ڿ�
int shared_resource_flock = 0;
acl::fiber_lock lock;  // fiber_lock ���

// �ڿ��� �����ϴ� �ڷ�ƾ
void lock_fiber(int fiber_id) {
    for (int i = 0; i < 10; ++i) {
        lock.lock();  // fiber_lock Ȱ��ȭ
        std::cout << "Fiber " << fiber_id << " locked the resource at iteration " << i << std::endl;

        shared_resource_flock++;
        std::cout << "Fiber " << fiber_id << " accessed resource: " << shared_resource_flock << std::endl;

        lock.unlock();  // fiber_lock ����
        std::cout << "Fiber " << fiber_id << " unlocked the resource at iteration " << i << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


// �� �����忡�� �ڷ�ƾ�� �����ϴ� �Լ�
static void thread_func(int thread_id) {
    for (int i = 1; i <= 5; ++i) {   // 5���� �ڷ�ƾ ����
        go[i]() {
            lock_fiber(i);
        };
    }

    acl::fiber::schedule();
}

int main_flock() {
    // ACL �ʱ�ȭ
    acl::acl_cpp_init();

    // 3���� ������ ����
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(thread_func, i);  // �� �����忡 �ڷ�ƾ ���� �Լ� �Ҵ�
    }

    // ��� �����尡 ����� ������ ���
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();  // ������ ���� ���
        }
    }

    std::cout << "Final shared resource value: " << shared_resource_flock << std::endl; // ���� 50

    return 0;
}
