#pragma once
#include <windows.h>

class Handle
{
    // class that store a handle and handle destructor
  public:
    Handle(HANDLE h);
    Handle();
    Handle(const Handle& h) = delete;
    Handle(Handle&& h) noexcept;


    Handle& operator=(Handle&& h) noexcept;
    Handle& operator=(const Handle& h) noexcept = delete;

    virtual ~Handle();

    HANDLE getHandle() const;
    void setHandle(HANDLE h);

  private:
    HANDLE m_handle;
};

