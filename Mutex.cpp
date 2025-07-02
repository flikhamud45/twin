#include "Main.h"
#include "Mutex.h"



Mutex::Mutex(const HANDLE h) : m_handle(h) {
    // blank intentionally
}


Mutex::Mutex() : m_handle(NULL) {
    //blank
}

Mutex::Mutex(const char* name)
    : m_handle(CreateMutexA(NULL, TRUE, name)) {
    if (m_handle.getHandle() == NULL) {
        throw WinapiException("CreateMutexA",
                              WinapiErrornoMethod::standardError);
    }
    
}


Mutex::Mutex(Mutex&& m) noexcept : m_handle(getHandle()) {
    m.m_handle.setHandle(NULL);
}


Mutex& Mutex::operator=(Mutex&& other) noexcept {
    m_handle = std::move(other.m_handle);
    return *this;
}


HANDLE Mutex::getHandle() {
    return m_handle.getHandle();
}


Mutex::~Mutex() {
    if (getHandle() != NULL) {
        if (!ReleaseMutex(getHandle())) {
            std::cout << "error number " << GetLastError()
                      << " in ReleaseMutex\n";
        }
    }
}
