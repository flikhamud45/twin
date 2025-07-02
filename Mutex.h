#pragma once
#include "Handle.h"
#include <iostream>

class Mutex {
    // class that store a mutex and handle destructor
  public:
    Mutex(HANDLE h);
    Mutex();
    Mutex(const char* name);

    Mutex& operator=(Mutex&& other) noexcept;

    HANDLE getHandle() const;

    ~Mutex();

private:
    Handle m_handle;
};

