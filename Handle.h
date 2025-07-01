#pragma once
#include <windows.h>

class Handle
{
    // class that store a handle and handle destructor
  public:
    Handle(HANDLE h);

    virtual ~Handle();

    HANDLE getHandle();

  protected:
    HANDLE m_handle;
};

