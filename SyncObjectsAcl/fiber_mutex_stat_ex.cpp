#include "pch.h"
#include <unordered_map>
#include <mutex>
#include <chrono>

// ����� ���� ������ ���� ����� ���� Ŭ����
class fiber_mutex_stat_tracker {
public:
    void add_mutex(acl::fiber_mutex* mtx) {
        std::lock_guard<std::mutex> lock(mutex_);
        tracked_mutexes_[mtx] = false;  // ���ؽ��� ��� �������� ����
    }

    void lock_mutex(acl::fiber_mutex* mtx, int fiber_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        tracked_mutexes_[mtx] = true;  // ���ؽ��� ��ݵǾ����� ���
        std::cout << "Fiber " << fiber_id << " locked the mutex.\n";
    }

    void unlock_mutex(acl::fiber_mutex* mtx, int fiber_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        tracked_mutexes_[mtx] = false;  // ���ؽ��� �����Ǿ����� ���
        std::cout << "Fiber " << fiber_id << " unlocked the mutex.\n";
    }

    void deadlock_check() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : tracked_mutexes_) {
            if (pair.second) {
                std::cout << "Deadlock detected! A mutex is still locked.\n";
            }
        }
    }

private:
    std::mutex mutex_;
    std::unordered_map<acl::fiber_mutex*, bool> tracked_mutexes_;  // ���ؽ� ��� ���� ����
};

// ���ؽ��� ����� ���� ����� ���� ��ü
acl::fiber_mutex mutex_ms;
fiber_mutex_stat_tracker mutex_stat;

void deadlock_fiber1(int fiber_id) {
    mutex_ms.lock();
    mutex_stat.lock_mutex(&mutex_ms, fiber_id);
    std::this_thread::sleep_for(std::chrono::seconds(1));  // �������� ����� ���� ���ɼ�
    mutex_stat.unlock_mutex(&mutex_ms, fiber_id);
    mutex_ms.unlock();
}

void deadlock_fiber2(int fiber_id) {
    mutex_ms.lock();
    mutex_stat.lock_mutex(&mutex_ms, fiber_id);
    std::this_thread::sleep_for(std::chrono::seconds(1));  // �������� ����� ���� ���ɼ�
    mutex_stat.unlock_mutex(&mutex_ms, fiber_id);
    mutex_ms.unlock();
}

int main() {
    acl::acl_cpp_init();

    // ����� ���� ���� ����
    mutex_stat.add_mutex(&mutex_ms);

    go[&]() {
        deadlock_fiber1(1);
    };
    go[&]() {
        deadlock_fiber2(2);
    };

    acl::fiber::schedule();

    // ����� ���� ���
    mutex_stat.deadlock_check();  // ����� ���� Ȯ��
    return 0;
}
