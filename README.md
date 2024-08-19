# CPP-Socket-acl

이 프로젝트는 Visual C++ 환경에서 오픈 소스 네트워크 프레임워크인 Acl(Advanced C/C++ Library)을 활용한 소켓 프로그래밍 학습을 목표로 하고 있습니다.

<br>

# 목차
1. [ACL 이란?](#acl-이란)
   - ACL 간단 소개
   - ACL 주요 기능 소개
   - 이 깃허브에서 다루는 실습 기능 소개

2. [ACL 빌드 및 정적 라이브러리로 추가하는 방법](#acl-빌드-및-정적-라이브러리로-추가하는-방법)

3. [ACL 간단 실습](#acl-간단-실습)
   - TCP Eco 서버 (TCP Eco Server + CppConsoleClient)
   - Coroutine을 사용한 TCP Eco 서버 (TCP Coroutine Eco Server + CppConsoleClient)
   - Redis와 상호작용하는 서버 (Redis Server)

4. [ACL 채팅 서버 실습 (with C# WinForms client)](#acl-채팅-서버-실습-with-c-winforms-client)
   - 실습 목표 소개
   - 주요 기능
      + Redis를 사용한 로그인
      + 채팅 기능
      + C#과 C++ 사이의 패킷 직렬화 기법
   - 실습 캡쳐
  
<br>

# ACL 이란?

[The Acl (Advanced C/C++ Library) project](https://github.com/acl-dev/acl/tree/master)은 강력한 다중 플랫폼 네트워크 통신 라이브러리 및 서비스 프레임워크입니다.

지원되는 플랫폼:
- Linux, Windows, Solaris, FreeBSD, MacOS, iOS, AndroidOS

## ACL 주요 기능 소개
ACL 라이브러리에서 제공하는 주요 기능은 다음과 같습니다:

1. **Basic Network Module**
   - 가장 기본적인 스트리밍 통신 모듈로, 네트워크 스트리밍을 지원할 뿐만 아니라 파일 스트리밍도 지원

2. **Coroutine (코루틴)**
   - 비동기 코드를 작성할 수 있는 경량 코루틴 지원

3. **HTTP Module**
   - 고성능 HTTP 서버 및 클라이언트 구현 가능

4. **Redis Client**
   - Redis와의 상호작용을 위한 고성능 클라이언트 모듈 제공

5. **MQTT Module**
   - IoT 장치와의 통신을 위한 MQTT 모듈 제공

6. **Server Framework**
   - 다양한 서버 아키텍처를 위한 템플릿 및 기본 구조 제공

이 깃허브 프로젝트에서는 ACL을 사용하여 TCP 서버와 클라이언트, Redis 연동, 코루틴 기반 서버, 그리고 C# WinForms 클라이언트와의 연동을 실습합니다.

<br>

# ACL 빌드 및 정적 라이브러리로 추가하는 방법

## 1. ACL 빌드
1. 이 리포지토리의 [acl 디렉토리](./acl)로 이동합니다. 또는 [공식 깃허브](https://github.com/acl-dev/acl)를 clone 합니다.
2. 빌드를 진행하세요.
   - windows 기준 : `acl_cpp_vc2022.sln` 솔루션에서 필요한 각 프로젝트의 빌드를 진행한다. <br>
      * lib_acl
      * lib_protocol
      * lib_acl_cpp
      * libfiber
      * libfiber_cpp
   - 빌드 방법에 대한 자세한 내용은 [Acl 빌드 후 정적 라이브러리로 추가하기](./Document/Build.md)를 참조하세요.

## 2. 정적 라이브러리 추가
정적 라이브러리를 Visual C++ 프로젝트에 추가하는 방법은 [친절한 정적 라이브러리 추가 방법](./Document/HowToAddCPPStaticLibrary.md)에 사진과 함께 설명되어 있습니다.
   - 플랫폼 확인 (ex. win32)
   - C/C++ > 일반 > **추가 포함 디렉터리**에 헤더파일 경로 추가
   - 링커 > 일반 > **추가 라이브러리 디렉터리**에 lib 파일 경로 추가
   - 링커 > 입력 > **추가 종속성**에 lib파일 이름 추가

<br>

# ACL 간단 실습

## 1. TCP Eco Server + CppConsoleClient

이 실습은 기본적인 TCP Echo 서버와 C++로 구현된 간단한 콘솔 클라이언트를 다룹니다. 서버는 클라이언트로부터 받은 데이터를 그대로 돌려줍니다.

1. 서버 코드는 `TCPEcoServerAcl/` 디렉토리에 있습니다. [🖱️click](./TCPEcoServerAcl)
2. 클라이언트 코드는 `CppConsoleClient/` 디렉토리에 있습니다. [🖱️click](./CppConsoleClient)

## 2. TCP Coroutine Eco Server + CppConsoleClient

이 실습은 TCP Echo 서버를 코루틴 기반으로 확장합니다. 이 서버는 경량 코루틴을 사용하여 클라이언트와 비동기적으로 통신합니다.

1. 서버 코드는 `TCPCoroutineEcoServerAcl/` 디렉토리에 있습니다. [🖱️click](./TCPCoroutineEcoServerAcl)
2. 클라이언트 코드는 `CppConsoleClient/` 디렉토리에 있습니다. [🖱️click](./CppConsoleClient)

## 3. Redis Server

ACL을 사용하여 Redis와 상호작용하는 서버를 구축합니다. 이 예제는 Redis 클라이언트 모듈을 사용하여 데이터베이스와 통신합니다.

1. Redis 서버 코드는 `RedisServerAcl/` 디렉토리에 있습니다. [🖱️click](./RedisServerAcl)

<br>

# ACL 채팅 서버 실습 (with C# WinForms client)

1. 서버 코드는 `ChatServerAcl/` 디렉토리에 있습니다. [🖱️click](./ChatServerAcl)
2. 클라이언트 코드는 `WinFormsClient/` 디렉토리에 있습니다. [🖱️click](./WinFormsClient)

## 실습 목표 소개

이 실습의 목표는 C++ ACL을 사용하여 서버를 구축하고, C# WinForms 기반의 클라이언트와 연동하는 것입니다. <br>
이 과정에서 Redis를 사용한 사용자 로그인, 채팅 메시지 처리, 패킷 직렬화 및 역직렬화 등의 개념을 다룹니다.

## 주요 기능

1. **Redis를 사용한 로그인**: 사용자 인증은 Redis 데이터베이스에서 관리됩니다. 사용자가 로그인하면 서버는 Redis에서 해당 사용자의 정보를 확인합니다.

2. **채팅 기능**: 여러 사용자가 채팅방에 입장하여 메시지를 주고받을 수 있습니다. 서버는 각 사용자의 메시지를 해당 방에 있는 모든 사용자에게 브로드캐스트합니다.

3. **C#과 C++ 사이의 패킷 직렬화 기법**
   - **Protocol Buffers**: [Protocol Buffers](https://protobuf.dev/)를 사용하여 C#과 C++ 간의 데이터 직렬화/역직렬화를 보다 쉽게 할 수 있습니다.
   - **현재 구현 방식**: 이 프로젝트에서는 Protocol Buffers 같은 오픈소스 직렬화 데이터 구조를 사용하지 않고, <br>
     직접 메모리 버퍼에 쓰고 읽는 식으로 구현한 직렬화 방식으로 패킷을 주고받습니다. <br>
     이는 사용자 정의된 간단한 직렬화 포맷을 사용하여 데이터를 처리합니다.

## 실습 캡쳐

채팅 프로젝트의 실행 결과를 캡처한 화면입니다:

![실습 캡쳐 이미지](./Document/img/ChatServerExample.PNG) 

<br>

# 문서

## 프로젝트 시작 이전
- [오픈소스 라이브러리 추가 방법](./Document/Build.md)

## 실습 예제
- [코루틴 설명 및 예제](./Document/Coroutine.md)

## 원문 번역
- [예제 디렉토리 설명-번역](./Document/SAMPLES-KO.md)

