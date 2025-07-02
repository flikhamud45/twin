
#include "AddrInfo.h"

#include <ws2tcpip.h>

AddrInfo::AddrInfo(PADDRINFOA addr) : m_addr(addr) {
    // blank intentionally
}

AddrInfo::AddrInfo() : m_addr(NULL) {
    // blank
}


PADDRINFOA AddrInfo::getAddr() {
    return m_addr;
}

AddrInfo::~AddrInfo() {
    if (m_addr != NULL) {
        freeaddrinfo(m_addr);
    }
}
