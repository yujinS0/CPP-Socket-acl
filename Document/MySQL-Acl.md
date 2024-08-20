# Acl을 활용해 MySQL 쉽게 접근하기

### Acl 빌드 및 관련 정적 라이브러리 추가
Acl 사용을 위해 당연히 추가해줘야한다.
[ACL 빌드 및 정적 라이브러리 추가 방법](https://github.com/yujinS0/CPP-Socket-acl/tree/main?tab=readme-ov-file#acl-빌드-및-정적-라이브러리로-추가하는-방법)

- [오픈소스 라이브러리(Acl 빌드 후) 정적 라이브러리로 추가 방법](./Build.md)
- [친절한 정적 라이브러리 추가 방법](./HowToAddCPPStaticLibrary.md)


### mysql 관련 라이브러리 추가

우선 Acl을 사용하더라도, mysql 연결을 해주는 libmysql 자체는 따로 추가를 해줘야한다. <br>
테스트를 위해서도 MySQL은 필요하니 공식 홈페이지에서 MySQL Workbench나 MySQL Server 설치를 진행하자.  <br><br>
이때, MySQL 공식 홈페이지에서 다운 받을 때 Server까지 설치해주면, <br>
`C:\Program Files\MySQL\MySQL Server 8.0\include` 해당 경로에 이미 **헤더파일**이 있고, <br>
`C:\Program Files\MySQL\MySQL Server 8.0\lib` 에는 **정적 라이브러리(.lib)**, **동적 라이브러리(.dll)** 파일이 있을 것이다. <br>


 <br> <br>


### Visual Studio에서 MySQL 프로젝트 설정 방법 (Windows 환경)

#### 1. **라이브러리 및 헤더 파일 설정**

1. **헤더 파일 경로 추가**:
   - Visual Studio에서 프로젝트를 열고, 프로젝트 속성 창을 엽니다.
   - **C/C++ -> 일반 -> 추가 포함 디렉터리**로 이동합니다.
   - MySQL Server 설치 경로 내의 `include` 폴더 (`C:\Program Files\MySQL\MySQL Server 8.0\include`)를 추가합니다.

2. **라이브러리 파일 경로 추가**:
   - 같은 속성 창에서 **링커 -> 일반 -> 추가 라이브러리 디렉터리**로 이동합니다.
   - MySQL Server 설치 경로 내의 `lib` 폴더 (`C:\Program Files\MySQL\MySQL Server 8.0\lib`)를 추가합니다.
   
3. **링커 입력 설정**:
   - **링커 -> 입력 -> 추가 종속성** 항목에 `libmysql.lib`를 추가합니다.

#### 2. **libmysql.dll 설정**

1. MySQL 클라이언트 라이브러리인 `libmysql.dll` 파일을 실행 파일 디렉토리로 복사해야 합니다. 이는 프로젝트가 MySQL과 통신할 때 필요합니다. 
   - `C:\Program Files\MySQL\MySQL Server 8.0\lib` 디렉토리 내에 `libmysql.dll` 파일이 있습니다.
   - 이를 **실행 파일 디렉토리**로 복사합니다. ex. Debug 디렉토리

#### 3. **MySQL 코드 작성 및 테스트**

작성중

