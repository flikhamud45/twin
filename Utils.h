#include "main.h"

constexpr char MUTEX_NAME[] = "technai_mutex";
constexpr char RUN_REG[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr WCHAR PROGRAM_NAME[] = L"Technai";
constexpr WCHAR PROGRAM_PATH[] =
    L"C:\\Users\\User\\source\\repos\\twin\\x64\\Debug\\twin.exe";
constexpr char DEFAULT_MSG[] = "MANAGMENT PROGRAM IS UP";
constexpr char DEFAULT_TITLE[] = "MANAGMENT PROGRAM";


// ensure that there is only one program running and return a locked mutex.
Mutex ensureOneProgram();

// make the program run on startup
void runOnStartUp(const std::wstring& path = PROGRAM_PATH);

// open a message bos
void openMessageBox(const char* msg = DEFAULT_MSG,
                    const char* title = DEFAULT_TITLE);