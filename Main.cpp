#include "Main.h"
#include "Handle.h"
#include "Mutex.h"
#include <iostream>
#include <windows.h>

constexpr char MUTEX_NAME[] = "technai_mutex";
constexpr char RUN_REG[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr WCHAR PROGRAM_NAME[] = L"Technai";
constexpr WCHAR PROGRAM_PATH[] = L"C:\\Users\\User\\source\\repos\\twin\\x64\\Debug\\twin.exe";
constexpr char DEFAULT_MSG[] = "MANAGMENT PROGRAM IS UP";
constexpr char DEFAULT_TITLE[] = "MANAGMENT PROGRAM";

WinapiException::WinapiException(const char* lastFunc, WinapiError error) : m_lastFunc(lastFunc) {
    switch (error) {
    case WinapiError::standartError:
        m_errorno = GetLastError();
        break;
    default:
        m_errorno = 0;
        break;
    }
}

WinapiException::WinapiException(const char* lastFunc, int errorno) : m_lastFunc(lastFunc), m_errorno(errorno) {
    // blank
}


int WinapiException::getErrorno() {
    return m_errorno;
}

const char* WinapiException::getLastFunc() {
    return m_lastFunc;
}



Mutex ensureOneProgram() {
    // ensure that there is only one program running and return a locked mutex.
    HANDLE mutex = CreateMutexA(NULL, TRUE, MUTEX_NAME);

    if (mutex == NULL) {
        throw WinapiException("CreateMutexA", WinapiError::standartError);
    }

    DWORD waitStatus = WaitForSingleObject(mutex, 0);
    if (waitStatus == WAIT_OBJECT_0 || waitStatus == WAIT_ABANDONED) {
        return mutex;
    } else if (waitStatus == WAIT_TIMEOUT) {
        std::cout << "program is already running...\n";
        exit(1);
    } else if (waitStatus == waitStatus) {
        throw WinapiException("WaitForSingleObject", WinapiError::standartError);
    } else {
        std::cout << "Unknown error\n";
        exit(1);
    }
}


void runOnStartUp() {
    HKEY hkey;

    LSTATUS status = RegCreateKeyA(HKEY_CURRENT_USER, RUN_REG, &hkey);

    if (status != ERROR_SUCCESS) {
        throw WinapiException("RegCreateKeyA", status);
    }

    status = RegSetKeyValueW(
        hkey,
        NULL,
        PROGRAM_NAME, 
        REG_SZ, 
        PROGRAM_PATH,
        static_cast<DWORD>(wcslen(PROGRAM_PATH)) + 1 // including the null terminator as needed according to the doc of the function
    );
    RegCloseKey(hkey);
    if (status != ERROR_SUCCESS) {
        throw WinapiException("RegSetKeyValueW", status);
    }
}

void openMessageBox(const char* msg = DEFAULT_MSG, const char* title = DEFAULT_TITLE) {
    int msgbox = MessageBoxA(
        NULL,
        msg,
        title, 
        MB_OK
    );
    if (msgbox == NULL) {

        throw WinapiException("MessageBoxA", WinapiError::standartError);
    }
}

int main() {
    try {
        Mutex m = ensureOneProgram();
        runOnStartUp();
        openMessageBox();
    } catch (WinapiException& e) {
        std::cout << "error number " << e.getErrorno() << " in "
                  << e.getLastFunc() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Unknown exception: " << e.what() << "\n";
    } catch (...) {
        std::cout << "Unknown exception\n";
    }
}

