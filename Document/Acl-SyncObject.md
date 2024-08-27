# Acl에서 제공하는 동기화 객체

## 개요
- acl_fiber란? [📄Acl fiber의 Coroutine](./Coroutine.md)
- 동기화 객체란? [📄동기화 객체](./Cpp-SynchronizationObject.md)

acl_fiber에서 제공하는 경량 스레드의 일종인 coroutine 사용 시, 동기화를 위해 지원되는 동기화 객체에 대한 문서

------------------------------------------------

## fiber_mutex
- 코루틴 뮤텍스 객체
- 하나의 코루틴이 자원에 접근하는 동안, 다른 코루틴이 해당 자원에 접근하지 못하도록 상호 배제를 보장합니다.

- 사용 예시 : 코루틴 간 자원 접근 제어

- 구현
  ```cpp
  bool fiber_mutex::lock(void) { acl_fiber_mutex_lock(mutex_); return true; }
  bool fiber_mutex::unlock(void) { acl_fiber_mutex_unlock(mutex_); return true; }
  ```

- 코드 예시
  ```cpp
  acl::fiber_mutex mutex;
  mutex.lock();
  // 자원 접근
  mutex.unlock();
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_mutex](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_mutex.cpp)
- [📄더 자세한 fiber_mutex 사용법이 궁금해!](./Coroutine-fiber_mutex.md)

<br>

------------------------------------------------

## fiber_event
- 코루틴 간 신호를 전달하여, 특정 조건이 발생했을 때 작업을 트리거하는 이벤트 객체
  + 이벤트 발생 시 대기 중인 코루틴을 깨움

- 사용 예시 : 비동기 작업을 수행하는 코루틴 간 이벤트를 기반으로 동기화하는 경우

  
- 구현
```cpp
bool fiber_event::wait(void) { return acl_fiber_event_wait(event_); }
bool fiber_event::notify(void) { return acl_fiber_event_notify(event_); }
```

- 코드 예시
  ```cpp
  acl::fiber_event event;
  event.wait();   // 다른 코루틴이 이벤트를 발생시킬 때까지 대기
  event.notify(); // 이벤트 발생
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_event](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_event.cpp)



<br>

------------------------------------------------

## fiber_lock
- 일반적인 락 메커니즘 제공
  + 간단한 상호 배제 락 (코루틴 간 자원 보호를 위한)
- Q. 이때, fiber_mutex랑 다른 점이 무엇이냐!



- 사용 예시 : 공유 자원에 대한 단순한 보호와 접근 관리

  
- 구현
  ```cpp
  bool fiber_lock::lock(void) { acl_fiber_lock_lock(lock_); return true; }
  bool fiber_lock::unlock(void) { acl_fiber_lock_unlock(lock_); return true; }
  ```

- 코드 예시
  ```cpp
  acl::fiber_lock lock;
  lock.lock();   // 락 획득
  // 자원 접근
  lock.unlock(); // 락 해제
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_lock](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_lock.cpp)

<br>

------------------------------------------------

## fiber_rwlock 
- 읽기-쓰기 락 객체
- Read의 경우 동시에 여러 코루틴을 허용하고, Write의 경우 하나의 코루틴만 허용
  


- 사용 예시 : 읽기 작업이 많고, 쓰기 작업은 드물게 발생하는 경우

  
- 구현
  ```cpp
  void fiber_rwlock::rlock(void) { acl_fiber_rwlock_rlock(rwlk_); }
  void fiber_rwlock::wlock(void) { acl_fiber_rwlock_wlock(rwlk_); }
  ```

- 코드 예시


- **관련 파일**: [🖱️Acl 소스코드 : fiber_rwlock](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_rwlock.cpp)


<br>

------------------------------------------------

## fiber_sem
- 코루틴 세마포어 객체
- 특정 자원에 접근할 수 있는 코루틴의 수를 제한하는 객체


- 사용 예시 : 자원에 대한 접근 횟수를 제한하는 상황에서 사용
  
- 구현
  ```cpp
  int fiber_sem::wait(void) { /* 세마포어 대기 */ }
  void fiber_sem::post(void) { /* 세마포어 해제 */ }
  ```

- 코드 예시
  ```cpp
  acl::fiber_sem sem(1);  // 세마포어 초기화
  sem.wait();   // 세마포어를 대기
  sem.post();   // 세마포어 해제
  ```


- **관련 파일**: [🖱️Acl 소스코드 : fiber_sem](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_sem.cpp)


<br>

------------------------------------------------

## fiber_cond
- 조건 만족 시까지 대기하는 코루틴을 깨우는 조건 변수 객체
- 특정 조건 만족 시 대기 중인 코루틴에게 신호를 보내 작업을 계속하게
- Q. 이때, fiber_event와 다른게 무엇이냐!


- 사용 예시 : 특정 이벤트나 조건이 발생할 때까지 코루틴을 일시 중단시키는 경우

  
- 구현
```cpp
  bool fiber_cond::wait(fiber_mutex& mutex, int timeout /* = -1 */) { return acl_fiber_cond_wait(cond_, mutex.get_mutex()) == 0; }
  bool fiber_cond::notify(void) { return acl_fiber_cond_signal(cond_) == 0; }
  ```

- 코드 예시
  ```cpp
  acl::fiber_cond cond;
  cond.wait();   // 조건이 만족될 때까지 대기
  cond.signal(); // 조건이 만족되었음을 신호
  ```

- **관련 파일**: [🖱️Acl 소스코드 : fiber_cond](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_cond.cpp)


<br>

------------------------------------------------

## fiber_mutex_stat
- 뮤텍스의 상태를 추적하여 데드락(교착상태)를 감지하는 객체

- 사용 예시 : 데드락 발생 시 진단 및 상태 출력


  
- 구현
```cpp
bool fiber_mutex::deadlock(fiber_mutex_stats& out) { /* 교착 상태 추적 */ }
void fiber_mutex::deadlock_show(void) { /* 교착 상태 출력 */ }
```

- 코드 예시



- **관련 파일**: [🖱️Acl 소스코드 : fiber_mutex_stat](https://github.com/acl-dev/acl/blob/master/lib_fiber/cpp/src/fiber_mutex_stat.cpp)


<br>

------------------------------------------------

## event_mutex
- 이벤트 뮤텍스 객체
- 코루틴 및 스레드 간의 상호 배제를 보장하며 이벤트를 동기화하는 메커니즘
  
- 구현


- 코드 예시

  ```cpp
  acl::event_mutex mutex;
  mutex.lock();   // 자원을 잠금
  mutex.unlock(); // 자원을 해제
  ```





<br>

------------------------------------------------

## event_timer

  
- 구현


- 코드 예시



