
#include "Handle.h"

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

