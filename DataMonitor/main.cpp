#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>
#include "sqlite/sqlite3.h"

struct Inventory {
    int         id;
    std::string partNumber;
    std::string partName;
    int         quantity;
    double      unitPrice;
    std::string updatedAt;
};

static const char* DB_PATH           = "../../../DataPersistence/DP/inventory.db";
static const int   LOW_STOCK_THRESH  = 100;
static const int   REFRESH_SEC       = 3;

// -----------------------------------------------------------------------
static std::vector<Inventory> fetchAll(sqlite3* db) {
    std::vector<Inventory> items;
    const char* sql =
        "SELECT id, part_number, part_name, quantity, unit_price, updated_at "
        "FROM inventory ORDER BY id;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return items;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Inventory it;
        it.id         = sqlite3_column_int(stmt, 0);
        it.partNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        it.partName   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        it.quantity   = sqlite3_column_int(stmt, 3);
        it.unitPrice  = sqlite3_column_double(stmt, 4);
        it.updatedAt  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        items.push_back(it);
    }
    sqlite3_finalize(stmt);
    return items;
}

// -----------------------------------------------------------------------
static void clearScreen() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(h, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;
    COORD origin = { 0, 0 };
    DWORD written;
    FillConsoleOutputCharacterA(h, ' ', cells, origin, &written);
    FillConsoleOutputAttribute(h, csbi.wAttributes, cells, origin, &written);
    SetConsoleCursorPosition(h, origin);
}

static void setColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// -----------------------------------------------------------------------
static void render(sqlite3* db, const char* dbPath) {
    auto items = fetchAll(db);

    int    totalQty    = 0;
    double totalValue  = 0.0;
    int    lowCount    = 0;
    for (const auto& it : items) {
        totalQty   += it.quantity;
        totalValue += static_cast<double>(it.quantity) * it.unitPrice;
        if (it.quantity < LOW_STOCK_THRESH) ++lowCount;
    }

    SYSTEMTIME st;
    GetLocalTime(&st);
    char timeStr[32];
    sprintf_s(timeStr, "%04d-%02d-%02d %02d:%02d:%02d",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    clearScreen();

    // ── 헤더 ──────────────────────────────────────────────────────────
    setColor(11);
    std::cout << "================================================================\n";
    std::cout << "         DataMonitor  -  Inventory Monitoring System            \n";
    std::cout << "================================================================\n";
    setColor(7);
    std::cout << "  Last Updated : " << timeStr << "\n";
    std::cout << "  DB Path      : " << dbPath  << "\n";
    std::cout << "  Refresh      : " << REFRESH_SEC << "s   |   [Q] Quit\n";
    setColor(11);
    std::cout << "----------------------------------------------------------------\n";

    // ── 테이블 헤더 ───────────────────────────────────────────────────
    setColor(14);
    printf("  %-4s | %-12s | %-20s | %6s | %12s | %-19s\n",
        "ID", "품번", "품명", "수량", "단가(원)", "최종수정");
    setColor(11);
    std::cout << "  -----+--------------+----------------------+--------+"
                 "--------------+--------------------\n";

    // ── 데이터 행 ─────────────────────────────────────────────────────
    for (const auto& it : items) {
        setColor(it.quantity < LOW_STOCK_THRESH ? 12 : 7);
        printf("  %-4d | %-12s | %-20s | %6d | %12.0f | %s\n",
            it.id,
            it.partNumber.c_str(),
            it.partName.c_str(),
            it.quantity,
            it.unitPrice,
            it.updatedAt.c_str());
    }

    // ── 요약 ──────────────────────────────────────────────────────────
    setColor(11);
    std::cout << "================================================================\n";
    setColor(14);
    printf("  총 SKU: %d개   |   총 수량: %d   |   총 재고가치: %.0f원\n",
        static_cast<int>(items.size()), totalQty, totalValue);

    if (lowCount > 0) {
        setColor(12);
        printf("  [WARNING] 부족 재고 (수량 < %d): %d개 항목  (위 빨간 행 확인)\n",
            LOW_STOCK_THRESH, lowCount);
    } else {
        setColor(10);
        std::cout << "  [OK] 모든 재고 정상\n";
    }
    setColor(11);
    std::cout << "================================================================\n";
    setColor(7);
}

// -----------------------------------------------------------------------
int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    const char* dbPath = (argc > 1) ? argv[1] : DB_PATH;

    sqlite3* db = nullptr;
    if (sqlite3_open(dbPath, &db) != SQLITE_OK) {
        std::cerr << "[ERROR] DB 열기 실패: " << sqlite3_errmsg(db) << "\n"
                  << "  경로: " << dbPath << "\n"
                  << "  사용법: DataMonitor.exe [DB경로]\n";
        sqlite3_close(db);
        return 1;
    }

    while (true) {
        render(db, dbPath);

        for (int i = 0; i < REFRESH_SEC * 10; ++i) {
            if (_kbhit()) {
                char c = static_cast<char>(_getch());
                if (c == 'q' || c == 'Q') {
                    setColor(7);
                    std::cout << "\n모니터링을 종료합니다.\n";
                    sqlite3_close(db);
                    return 0;
                }
            }
            Sleep(100);
        }
    }
}
