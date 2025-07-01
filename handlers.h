#pragma once
#include <windows.h>

class Handle
{
    // class that store a handle and handle destructor
  public:
    Handle(HANDLE h);

    ~Handle();

    HANDLE getHandle();

  protected:
    HANDLE m_handle;
};

class Mutex : Handle
{
    // class that store a mutex and handle destructor
  public:
    Mutex(HANDLE h);

    ~Mutex();
};
