#pragma once
#include "Handle.h"
#include <iostream>

class Mutex {
    // class that store a mutex and handle destructor
  public:
    Mutex(const HANDLE h);
    

    HANDLE getHandle() const;

    ~Mutex();

private:
    const Handle m_handle;
};

