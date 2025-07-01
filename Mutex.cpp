#include "Mutex.h"
#include <iostream>


Mutex::Mutex(HANDLE h) : Handle(h) {
    // blank intentionally
}

Mutex::~Mutex() {
    if (!ReleaseMutex(m_handle)) {
        std::cout << "error number " << GetLastError() << " in ReleaseMutex\n";
    }
}
