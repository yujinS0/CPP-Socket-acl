## Acl 빌드 후 정적 라이브러리로 추가하기

`acl_cpp_vc2022.sln`의 필요한 프로젝트 빌드 후, <br>
lib 파일과 hpp 파일의 경로를 바탕으로 정적 라이브러리로 추가해주면 된다. <br>

솔루션 속성에서 
* C/C++ > 일반 > 추가 포함 디렉터리에 헤더파일 경로 추가
  + `..\acl\lib_fiber\c\include;..\acl\lib_fiber\cpp\include;..\acl\lib_acl_cpp\include;..\acl\lib_protocol\include;..\acl\lib_acl\include;..\acl\include;%(AdditionalIncludeDirectories)`

* 링커 > 일반 > 추가 라이브러리 디렉터리에 lib 파일 경로 추가
  + `..\acl\Debug;%(AdditionalLibraryDirectories)`

* 링커 > 입력 > 추가 종속성에 lib파일 이름 추가
  + `lib_protocol.lib;lib_acl.lib;lib_acl_cpp.lib;libfiber.lib;libfiber_cpp.lib;$(CoreLibraryDependencies);%(AdditionalDependencies)`

<br>
이때 나의 경우, 빌드 파일과 플랫폼 모두 Win32(x64)로 진행했다.
