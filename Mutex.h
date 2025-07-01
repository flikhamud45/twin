#pragma once
#include "Handle.h"

class Mutex : Handle {
    // class that store a mutex and handle destructor
  public:
    Mutex(HANDLE h);

    virtual ~Mutex();
};