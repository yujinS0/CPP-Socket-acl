#include <iostream>
#include <string>

// �� ����ȭ ��ü�� main �Լ� ����
int main_fcond();   // fiber_cond ���� �Լ�
int main_fevent();  // fiber_event ���� �Լ�
int main_flock();   // fiber_lock ���� �Լ�
int main_fmutex();  // fiber_mutex ���� �Լ�
int main_sem();     // fiber_sem ���� �Լ�
int main_rw();      // fiber_rwlock ���� �Լ�
int main_ms();      // fiber_mutex_stat ���� �Լ�

int main() {
    std::string input;

    std::cout << "Select the synchronization object to test:\n";
    std::cout << "1. fiber_cond\n";
    std::cout << "2. fiber_event\n";
    std::cout << "3. fiber_lock\n";
    std::cout << "4. fiber_mutex\n";
    std::cout << "5. fiber_sem\n";
    std::cout << "6. fiber_rwlock\n";
    std::cout << "7. fiber_mutex_stat (deadlock check)\n";
    std::cout << "Enter your choice: ";
    std::cin >> input;

    if (input == "1") {
        return main_fcond();
    }
    else if (input == "2") {
        return main_fevent();
    }
    else if (input == "3") {
        return main_flock();
    }
    else if (input == "4") {
        return main_fmutex();
    }
    else if (input == "5") {
        return main_sem();
    }
    else if (input == "6") {
        return main_rw();
    }
    else if (input == "7") {
        return main_ms();
    }
    else {
        std::cout << "Invalid choice!" << std::endl;
        return 1;
    }
}
