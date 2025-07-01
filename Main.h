#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Mutex.h"
#include "Socket.h"
#include <Ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <exception>







BOOL wsaInitiated = FALSE;




enum class WinapiErrorType
{
    standardError = 1,
    wsaError
};

enum class ClientException {
    invalidMsg
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

