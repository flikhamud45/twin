#pragma once
#include <exception>
#include <windows.h>

constexpr char DEFAULT_PORT[]  = "12345";

BOOL wsaInitiated = FALSE;




enum class WinapiError
{
    standartError = 1,
    wsaError,
    otherError
};

class WinapiException : public std::exception
{
  public:
    WinapiException(const char* lastFunc, WinapiError error);

    WinapiException(const char* lastFunc, int errorno);

    int getErrorno();

    const char* getLastFunc();
    
  private:
    const char* m_lastFunc;
    int m_errorno;
};

