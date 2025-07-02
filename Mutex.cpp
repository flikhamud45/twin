#include "Mutex.h"



Mutex::Mutex(const HANDLE h) : m_handle(h) {
    // blank intentionally
}


Mutex::Mutex() : m_handle(NULL) {
    //blank
}

Mutex::Mutex(const char* name)
    : m_handle(CreateMutexA(NULL, TRUE, name)) {
    
}

Mutex& Mutex::operator=(Mutex&& other) noexcept {
    m_handle = other.m_handle;
    return *this;
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
