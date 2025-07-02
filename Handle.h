#pragma once
#include <windows.h>

class Handle
{
    // class that store a handle and handle destructor
  public:
    Handle(HANDLE h);
    Handle();
    Handle(const Handle& h) = delete;
    Handle(Handle&& h);


    Handle& operator=(Handle&& h);
    Handle& operator=(const Handle& h) = delete;

    virtual ~Handle();

    HANDLE getHandle() const;
    void setHandle(HANDLE h);

  private:
    HANDLE m_handle;
};

