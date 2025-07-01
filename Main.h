#pragma once
#include <exception>
#include <windows.h>
#define WIN32_LEAN_AND_MEAN

#include <Ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

constexpr char DEFAULT_PORT[]  = "12345";

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

