#pragma once
#include <exception>

enum class WinapiErrornoMethod
{
    standardError = 1, // GetLastError
    otherError 
};

class WinapiException : public std::exception
{
  public:
    WinapiException(const char* lastFunc, WinapiErrornoMethod error);

    WinapiException(const char* lastFunc, int errorno);

    int getErrorno();

    const char* getLastFunc();
    
  private:
    const char* m_lastFunc;
    int m_errorno;
};

