# ACL 심층 예제: Coroutine, MySQL, Redis 실습 가이드

Acl가 지원해주는 주요 기능을 더 자세히 다루는 예제 문서 (심층 예제)

<br>

## 목차
* ### Coroutine 심층 사용법
  + 사용 시, 멀티 스레드 환경에서의 사용을 위한 mutex, lock 등등
* ### mysql 사용법
* ### redis 심층 사용법
  + json 형식으로 저장하기 등등

<br><br>


## 예제 소스코드 정리

### Coroutine 관련 예제
* [samples](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c++)
  + event_mutex [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/event_mutex)
  + thread_cond [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_cond)
  + thread_event [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_event)
  + thread_lock [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_lock)
  + thread_mutex [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_mutex)
  + thread_tbox [소스코드](https://github.com/acl-dev/acl/tree/master/lib_fiber/samples-c%2B%2B/thread_tbox)
    
* 위 Samples은 [acl project](https://github.com/acl-dev/acl/) library의 APIs 사용


### MYSQL 관련 예제
* **2.52 mysql: mysql 클라이언트 프로그램** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/mysql)
* **2.53 mysql2: mysql 클라이언트 프로그램** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/mysql2)

### Redis 관련 예제
* **Redis 관련 파일** [소스코드](https://github.com/acl-dev/acl/tree/master/lib_acl_cpp/samples/redis)
  - 2.75 redis/redis_connection: redis 연결 클라이언트 프로그램
  - 2.76 redis/redis_hash: redis 해시 테이블과 유사한 클라이언트 애플리케이션
  - 2.77 redis/redis_hyperloglog: redis 하이퍼로그 클라이언트 프로그램
  - 2.78 redis/redis_key: redis 키 클라이언트 응용 프로그램
  - 2.79 redis/redis_list: redis 리스트 클라이언트 프로그램
  - 2.80 redis/redis_manager: redis 연결 풀 클러스터 관리 클라이언트 프로그램
  - 2.81 redis/redis_pool: redis 연결 풀 클라이언트 프로그램
  - 2.82 redis/redis_pubsub: redis 게시-구독 클라이언트 프로그램
  - 2.83 redis/redis_server: redis 서버 관리 클라이언트 애플리케이션
  - 2.84 REDIS/REDIS_SET: REDIS 수집 클라이언트 애플리케이션
  - 2.85 redis/redis_string: redis 문자열 클라이언트 애플리케이션
  - 2.86 redis/redis_trans: redis 트랜잭션 클라이언트
  - 2.87 redis/redis_zset: redis 정렬 집합 클라이언트 프로그램
  - 2.88 redis/redis_zset_pool: redis ordered-set 클라이언트, ordered-set 객체를 사용하여 대용량 데이터 블록을 저장하고 연결 풀링을 지원합니다.
