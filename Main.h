#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Mutex.h"
#include "Socket.h"
#include <Ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <exception>




enum class WinapiErrornoMethod {
    // indicates the type of method to get the last error
    standardError = 1,
    wsaError
};

enum class ClientException {
    invalidMsg,
    ClientDisconnected,
    invalidArgs
};

class WinapiException : public std::exception {

  public:
    // build an exception from the last func called that caused the exception
    // and the method to get the error number
    WinapiException(const char* lastFunc, WinapiErrornoMethod error);

    // build an exception from the last func called that caused the exception
    // and the actual error number
    WinapiException(const char* lastFunc, int errorno);

    int getErrorno();

    const char* getLastFunc();

  private:
    const char* m_lastFunc;
    int m_errorno;
};

