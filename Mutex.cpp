#include "Mutex.h"
#include <iostream>


Mutex::Mutex(const HANDLE h) : m_handle(h) {
    // blank intentionally
}


HANDLE Mutex::getHandle() const {
    return m_handle.getHandle();
}


Mutex::~Mutex() {
    if (!ReleaseMutex(m_handle.getHandle())) {
        std::cout << "error number " << GetLastError() << " in ReleaseMutex\n";
    }
}
