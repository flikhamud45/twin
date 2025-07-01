#include "Closers.h"
#include <iostream>


Handle::Handle(HANDLE h) : m_handle(h) {
    // blank intentionally
}

Handle::~Handle() {
    CloseHandle(m_handle);
}

HANDLE Handle::getHandle() {
    return m_handle;
}

Mutex::Mutex(HANDLE h) : Handle(h) {
    // blank intentionally
}


Mutex::~Mutex() {
    if (!ReleaseMutex(m_handle))
    {
        std::cout << "error number " << GetLastError() << " in ReleaseMutex\n";
    }
}
