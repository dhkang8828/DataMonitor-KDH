# DataMonitor

## 개요

[DataPersistence-KDH](https://github.com/dhkang8828/DataPersistence-KDH) 프로젝트에서 생성한 SQLite DB(`inventory.db`)에 쌓인 재고 데이터를 **3초마다 자동 갱신**하여 콘솔에 표시하는 모니터링 애플리케이션입니다.

## 화면 구성

```
================================================================
         DataMonitor  -  Inventory Monitoring System
================================================================
  Last Updated : 2026-06-12 14:30:00
  DB Path      : ../../../DataPersistence/DP/inventory.db
  Refresh      : 3s   |   [Q] Quit
----------------------------------------------------------------
  ID   | 품번         | 품명                 |   수량 |     단가(원) | 최종수정
  -----+--------------+----------------------+--------+--------------+--------------------
  1    | SEM-101      | DRAM 16GB DDR5       |   1500 |       8500 | 2026-06-12 14:00:00
  2    | SEM-102      | NAND Flash 512GB     |     42 |      12000 | 2026-06-12 14:00:00  ← 빨간색(부족)
  ...
================================================================
  총 SKU: 17개  |  총 수량: 12,345  |  총 재고가치: 123,456,789원
  [WARNING] 부족 재고 (수량 < 100): 3개 항목
================================================================
```

- 수량 100 미만 항목은 **빨간색** 강조
- 모든 재고 정상 시 `[OK]` 녹색 메시지 표시

## 연관 프로젝트

- **DataPersistence-KDH**: https://github.com/dhkang8828/DataPersistence-KDH
  - 재고 데이터 수집 및 SQLite DB 저장 담당
  - 본 프로젝트는 해당 DB를 읽기 전용으로 열어 데이터를 모니터링합니다.

## 기술 스택

| 항목 | 내용 |
|------|------|
| 언어 | C++20 |
| DB | SQLite 3 (소스 내장) |
| 플랫폼 | Windows (x64) |
| 빌드 도구 | Visual Studio 2022 (MSBuild v145) |
| 구성 | Debug / Release |

## 프로젝트 구조

```
DataMonitor/
├── main.cpp           # 모니터링 루프, 콘솔 렌더링
└── sqlite/
    ├── sqlite3.h      # SQLite 헤더
    └── sqlite3.c      # SQLite 소스 (amalgamation)
```

## 빌드 방법

1. `DataMonitor.slnx`를 Visual Studio 2022에서 엽니다.
2. 플랫폼 `x64`, 구성 `Debug` 또는 `Release` 선택
3. **빌드 > 솔루션 빌드** (`Ctrl+Shift+B`)

## 실행 방법

```bash
# 기본 실행 (DB 경로: ../../../DataPersistence/DP/inventory.db)
DataMonitor.exe

# DB 경로 직접 지정
DataMonitor.exe C:\path\to\inventory.db
```

| 키 | 동작 |
|----|------|
| `Q` | 모니터링 종료 |

## 동작 방식

1. 지정된 경로의 `inventory.db`에 SQLite로 연결
2. `inventory` 테이블 전체를 3초마다 `SELECT`
3. 재고 현황(품번·품명·수량·단가·수정일시) 및 요약 통계를 콘솔에 렌더링
4. 수량 100 미만 항목 경고 표시
