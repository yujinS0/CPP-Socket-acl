#include <iostream>
#include <string>

// 각 동기화 객체의 main 함수 선언
int main_fcond();   // fiber_cond 예제 함수
int main_fevent();  // fiber_event 예제 함수
int main_flock();   // fiber_lock 예제 함수
int main_fmutex();  // fiber_mutex 예제 함수
int main_sem();     // fiber_sem 예제 함수
int main_rw();      // fiber_rwlock 예제 함수
int main_ms();      // fiber_mutex_stat 예제 함수

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
