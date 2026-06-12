# DataMonitor

## 개요

[DataPersistence-KDH](https://github.com/dhkang8828/DataPersistence-KDH) 프로젝트에서 생성한 DB에 쌓인 데이터를 실시간으로 모니터링하는 C++ 콘솔 애플리케이션입니다.

## 연관 프로젝트

- **DataPersistence-KDH**: https://github.com/dhkang8828/DataPersistence-KDH
  - 데이터 수집 및 DB 저장 담당
  - 본 프로젝트는 해당 DB를 읽어 데이터 상태를 모니터링합니다.

## 기술 스택

- **언어**: C++20
- **플랫폼**: Windows (Win32 / x64)
- **빌드 도구**: Visual Studio 2022 (MSBuild v145)
- **구성**: Debug / Release

## 빌드 방법

1. `DataMonitor.slnx`를 Visual Studio 2022에서 엽니다.
2. 구성(Debug/Release)과 플랫폼(Win32/x64)을 선택합니다.
3. **빌드 > 솔루션 빌드** (`Ctrl+Shift+B`)

## 실행 방법

빌드 후 생성된 실행 파일을 실행합니다.

```
DataMonitor.exe
```
