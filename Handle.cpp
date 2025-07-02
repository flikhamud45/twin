
#include "Handle.h"


Handle::Handle(HANDLE h) : m_handle(h) {
    // blank intentionally
}

Handle::Handle() : m_handle(NULL) {
    // blank
}

Handle::~Handle() {
    if (m_handle != NULL) {
        CloseHandle(m_handle);
    }
}

HANDLE Handle::getHandle() const {
    return m_handle;
}

Handle::Handle(const Handle& h) : m_handle(h.m_handle){
    
}

Handle& Handle::operator=(Handle&& h) noexcept {
    m_handle = h.m_handle;
    return *this;
}


