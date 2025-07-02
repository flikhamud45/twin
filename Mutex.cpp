#include "Mutex.h"



Mutex::Mutex(const HANDLE h) : m_handle(h) {
    // blank intentionally
}

Mutex::Mutex() : m_handle(NULL) {
    //blank
}


HANDLE Mutex::getHandle() const {
    return m_handle.getHandle();
}


Mutex::~Mutex() {
    if (m_handle.getHandle() != NULL) {
        if (!ReleaseMutex(m_handle.getHandle())) {
            std::cout << "error number " << GetLastError()
                      << " in ReleaseMutex\n";
        }
    }
}
