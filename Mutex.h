#pragma once
#include "Handle.h"

class Mutex {
    // class that store a mutex and handle destructor
  public:
    Mutex(const HANDLE h);
    Mutex(const char* name);

    HANDLE getHandle() const;

    ~Mutex();

private:
    const Handle m_handle;
};
