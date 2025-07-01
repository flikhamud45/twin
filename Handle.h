#pragma once
#include <windows.h>

class Handle
{
    // class that store a handle and handle destructor
  public:
    Handle(HANDLE h);
    Handle(const Handle& h);

    Handle& operator=(Handle&& h) noexcept;

    ~Handle();

    HANDLE getHandle() const;

  protected:
    HANDLE m_handle;
};

