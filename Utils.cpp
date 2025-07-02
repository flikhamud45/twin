#include "main.h"
#include "Utils.h"
#include <Msi.h>
#pragma comment(lib, "msi.lib")

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

void runOnStartUp(const std::string& path) {
    HKEY hkey;

    LSTATUS status = RegCreateKeyA(HKEY_CURRENT_USER, RUN_REG, &hkey);

    if (status != ERROR_SUCCESS) {
        throw WinapiException("RegCreateKeyA", status);
    }
    CHAR* cpath = new CHAR[path.length()+1];
    strcpy_s(cpath, path.length()+1, path.c_str());
    status = RegSetKeyValueA(
        hkey, NULL, PROGRAM_NAME_A, REG_SZ, cpath,
                        static_cast<DWORD>(strlen(cpath)) * sizeof(char) +
            1 // including the null terminator as needed
              // according to the doc of the function
    );
    RegCloseKey(hkey);
    delete[] cpath;
    if (status != ERROR_SUCCESS) {
        throw WinapiException("RegSetKeyValueW", status);
    }
}

void runOnStartUp(const std::wstring& path) {
    HKEY hkey;

    LSTATUS status = RegCreateKeyA(HKEY_CURRENT_USER, RUN_REG, &hkey);

    if (status != ERROR_SUCCESS) {
        throw WinapiException("RegCreateKeyA", status);
    }
    WCHAR* cpath = new WCHAR[path.length() + 1];
    wcscpy_s(cpath, path.length() + 1, path.c_str());
    status =
        RegSetKeyValueW(hkey, NULL, PROGRAM_NAME, REG_SZ, cpath,
                        static_cast<DWORD>(wcslen(cpath)) * sizeof(wchar_t) +
                            1 // including the null terminator as needed
                              // according to the doc of the function
        );
    RegCloseKey(hkey);
    delete[] cpath;
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

void getFileHash(const std::string& fileName, PMSIFILEHASHINFO pHash) {
    memset(pHash, 0, sizeof(MSIFILEHASHINFO));
    pHash->dwFileHashInfoSize = sizeof(MSIFILEHASHINFO);
    char* cfileName = new char[fileName.length() + 1];
    strcpy_s(cfileName, fileName.length() + 1, fileName.c_str());
    UINT retCode = MsiGetFileHashA(cfileName, 0, pHash);
    if (retCode != ERROR_SUCCESS) {
        throw WinapiException("MsiGetFileHashA", retCode);
    }

}

BOOL isHashEqual(PMSIFILEHASHINFO pHash1, PMSIFILEHASHINFO pHash2) {
    return memcmp(pHash1, pHash2, sizeof(MSIFILEHASHINFO)) == 0;
}
