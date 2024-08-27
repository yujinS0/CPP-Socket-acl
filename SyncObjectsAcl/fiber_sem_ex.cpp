#include "pch.h"

// �������� ��ü
acl::fiber_sem sem(2);  // �ִ� �� ���� �ڷ�ƾ�� ���� ����
int shared_resource_sem = 0;

void sem_fiber(int fiber_id) {
    for (int i = 0; i < 5; ++i) {
        sem.wait();  // �������� ���
        std::cout << "Fiber " << fiber_id << " accessed the resource at iteration " << i << std::endl;

        shared_resource_sem++;
        std::cout << "Fiber " << fiber_id << " updated resource to: " << shared_resource_sem << std::endl;

        sem.post();  // �������� ����
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main_sem() {
    acl::acl_cpp_init();

    // 3���� �ڷ�ƾ ����
    for (int i = 0; i < 3; i++) {
        go[&]() {
            sem_fiber(i+1);
        };
    }
    
    acl::fiber::schedule();
    std::cout << "Final shared resource value: " << shared_resource_sem << std::endl;
    return 0;
}
