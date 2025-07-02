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

Mutex::Mutex(const Mutex& mutex) {
    m_handle = mutex.m_handle.getHandle();
}


Mutex& Mutex::operator=(Mutex&& other) noexcept {
    m_handle = std::move(other.m_handle);
    return *this;
}


HANDLE Mutex::getHandle() {
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
