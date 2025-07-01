#pragma once
#include <exception>

enum class WinapiErrorType
{
    standardError = 1,
    otherError
};

class WinapiException : public std::exception
{
  public:
    WinapiException(const char* lastFunc, WinapiErrorType error);

    WinapiException(const char* lastFunc, int errorno);

    int getErrorno();

    const char* getLastFunc();
    
  private:
    const char* m_lastFunc;
    int m_errorno;
};

