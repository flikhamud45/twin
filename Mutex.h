#pragma once
#include "Handle.h"
#include <iostream>

class Mutex {
    // class that store a mutex and handle destructor
  public:
    Mutex(HANDLE h);
    Mutex(const Mutex& mutex) = delete;
    Mutex();
    Mutex(const char* name);
    Mutex(Mutex&& m) noexcept;


    Mutex& operator=(Mutex&& other) noexcept;
    Mutex& operator=(const Mutex& other) = delete;

    HANDLE getHandle();

    ~Mutex();

private:
    Handle m_handle;
};

