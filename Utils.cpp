#include "main.h"
#include "Utils.h"

Mutex ensureOneProgram() {
    // ensure that there is only one program running and return a locked mutex.
    Mutex mutex(MUTEX_NAME);

    DWORD waitStatus = WaitForSingleObject(mutex.getHandle(), 0);
    if (waitStatus == WAIT_OBJECT_0 || waitStatus == WAIT_ABANDONED) {
        return mutex;
    } else if (waitStatus == WAIT_TIMEOUT) {
        std::cout << "program is already running...\n";
        exit(1);
    } else if (waitStatus == WAIT_FAILED) {
        throw WinapiException("WaitForSingleObject",
                              WinapiErrornoMethod::standardError);
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
        hkey, NULL, PROGRAM_NAME, REG_SZ, PROGRAM_PATH,
        static_cast<DWORD>(wcslen(PROGRAM_PATH)) * sizeof(wchar_t) +
            1 // including the null terminator as needed
              // according to the doc of the function
    );
    RegCloseKey(hkey);
    if (status != ERROR_SUCCESS) {
        throw WinapiException("RegSetKeyValueW", status);
    }
}

void openMessageBox(const char* msg, const char* title) {
    int msgbox = MessageBoxA(NULL, msg, title, MB_OK);
    if (msgbox == NULL) {
        throw WinapiException("MessageBoxA",
                              WinapiErrornoMethod::standardError);
    }
}
