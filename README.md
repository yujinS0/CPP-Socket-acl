# CPP-Socket-acl

Visual C++ 환경에서 오픈 소스 네트워크 프레임워크인 Acl을 활용한 소켓 프로그래밍 학습

## 프로젝트 구조

```
CPP-Socket-acl/
├── Client/
│   └── ... (클라이언트)
├── Document/
│   └── ... (문서 정리)
├── ServerAcl/
│   └── ... (서버)
└── acl/
    └── ... (ACL 라이브러리 관련 파일들)

```

------------------------------

## 코드 테스트를 위한 세팅
- acl/Debug 디렉토리로 .lib 파일들 옮기기 (.lib 파일은 acl/Debug_lib_Files에 존재)
 
## 초기 세팅
* [오픈소스 라이브러리 추가 방법](./Document/Build.md)


------------------------------

## Acl(Advanced C/C++ Library)
[The Acl (Advanced C/C++ Library) project](https://github.com/acl-dev/acl/tree/master) a is powerful multi-platform network communication library and service framework, 
<br>
suppoting LINUX, WIN32, Solaris, FreeBSD, MacOS, AndroidOS, iOS. 

* [github.com/jacking75/edu_cpp_server_programming/acl](https://github.com/jacking75/edu_cpp_server_programming/tree/main/acl)

<br>


### 중요한 6가지 모듈
* 1.1. Basic network module
* 1.2. Coroutine
* 1.3. HTTP module
* 1.4. Redis client
* 1.5. MQTT module
* 1.6. Server framework

------------------------------
## Coroutine(코루틴)
[코루틴이란?](./Document/Coroutine.md)  <br>
The coroutine module in Acl can be used on multiple platforms, and there are many engineering practices in some important projects.

* Run on Linux, MacOS, Windows, iOS and Android
* Support x86, Arm architecture
* Support select/poll/epoll/kqueue/iocp/win32 GUI message
* The DNS protocol has been implemented in acl coroutine, so DNS API can also be used in coroutine model
* Hook system IO API on Unix and Windows
    + Read API: read/readv/recv/recvfrom/recvmsg
    + Write API: write/writev/send/sendto/sendmsg/sendfile64
    + Socket API: socket/listen/accept/connect/setsockopt
    + event API: select/poll/epoll_create/epoll_ctl/epoll_wait
    + DNS API: gethostbyname/gethostbyname_r/getaddrinfo/freeaddrinfo
* Support shared stack mode to minimize memory usage
* Synchronization primitive
    + Coroutine mutex, semphore can be used between coroutines
    + Coroutine event can be used between coroutines and threads
* For more information, see [Using Acl fiber](https://github.com/acl-dev/acl/blob/master/lib_fiber/README_en.md)

## Redis
The redis client module in Acl is powerful, high-performance and easy to use.

* Support Bitmap/String/Hash/List/Set/Sorted Set/PubSub/HyperLogLog/Geo/Script/Stream/Server/Cluster/etc.
* Provides stl-like C++ interface for each redis command
* Automaticaly cache and adapt the changing of hash slots of the redis cluster on the client
* Communication in single, cluster or pipeline mode
* Connection pool be used in signle or cluster mode
* High performance in cluster or pipline mode
* Same interface for single, cluster and pipeline modes
* Retry automatically for the reason of network error
* Can be used in the shared stack coroutine mode
* For more information, see [Using Acl redis client](https://github.com/acl-dev/acl/blob/master/lib_acl_cpp/samples/redis/README.md)

--------------------------

## 문서

### 프로젝트 시작 이전
* [오픈소스 라이브러리 추가 방법](./Document/Build.md)

### 실습 예제
* [코루틴 설명 및 예제](./Document/Coroutine.md)

* [주요 예제 정리](./Document/EX.md)


### 원문 번역
* [예제 디렉토리 설명-번역](./Document/SAMPLES-KO.md) 






