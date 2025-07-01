
#include "AddrInfo.h"

#include <ws2tcpip.h>

AddrInfo::AddrInfo(PADDRINFOA addr) : m_addr(addr) {
    // blank intentionally
}

PADDRINFOA AddrInfo::getAddr() { return m_addr; }

AddrInfo::~AddrInfo() {
    freeaddrinfo(m_addr);
}
