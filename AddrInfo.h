#pragma once
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <iostream>


class AddrInfo {
  public:
    AddrInfo(PADDRINFOA addr);

    ~AddrInfo();

    PADDRINFOA getAddr();

  private:
    PADDRINFOA m_addr;
};
