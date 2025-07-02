#include "Main.h"
#include "Socket.h"

#include "Utils.h"

#include <winsock2.h>


Socket::Socket(SOCKET s) : m_socket(s) {
    // blank intentionally
    
}

Socket& Socket::operator=(Socket&& other) noexcept {
    m_socket = other.getSocket();
    other.m_socket = NULL;
    return *this;
}

Socket::~Socket() {
    if (m_socket != NULL) {
        closesocket(m_socket);
    }
}

Socket::Socket() : m_socket(NULL) {
    // blank
}


SOCKET Socket::getSocket() {
    return m_socket;
}
extern BOOL g_wsaInitiated = FALSE;

void wsaInit() {
    // init the wsa if needed
    if (!g_wsaInitiated) {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            throw WinapiException("WSAStartup", iResult);
        }
        g_wsaInitiated = TRUE;
    }
}

PADDRINFOA getServerAddr(const char* port) {
    wsaInit();
    // return a addrinfo of for the server

    struct addrinfo *result = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    int iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0) {
        throw WinapiException("getaddrinfo", iResult);
    }
    return result;
}

SOCKET createSocket(PADDRINFOA result) {
    // create a socket

    // Create a SOCKET for the server to listen for client connections
    SOCKET ListenSocket =
        socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        throw WinapiException("socket", WinapiErrornoMethod::wsaError);
    }
    return ListenSocket;
}

void bindSocket(PADDRINFOA result, const SOCKET& ListenSocket) {
    // bind a socket
    int iResult = bind(ListenSocket, result->ai_addr,
                       static_cast<int>(result->ai_addrlen));
    if (iResult == SOCKET_ERROR) {
        throw WinapiException("bind", WinapiErrornoMethod::wsaError);
    }
}


void listenSocket(const SOCKET& ListenSocket) {
    // listen on a socket (no blocking)

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        throw WinapiException("listen", WinapiErrornoMethod::wsaError);
    }
}


SOCKET acceptClient(const SOCKET& ListenSocket) {
    // wait for a client to connect and return its socket
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        throw WinapiException("accept", WinapiErrornoMethod::wsaError);
    }
    return ClientSocket;
}

ServerSocket::ServerSocket(const char* port) : m_addr(getServerAddr(port))  {
    m_ListenSocket = Socket(createSocket(m_addr.getAddr()));
    // blank
}

void ServerSocket::bind() {
    bindSocket(m_addr.getAddr(), m_ListenSocket.getSocket());
}

void ServerSocket::listen() {
    listenSocket(m_ListenSocket.getSocket());
}

SOCKET ServerSocket::accept() {
    return acceptClient(m_ListenSocket.getSocket());
}


ClientSocket::ClientSocket(SOCKET s) : m_sock(s) {
    // blank
}

void ClientSocket::recvall(char* recvbuf, int recvbuflen) {
    int iResult = recv(m_sock.getSocket(), recvbuf, recvbuflen, MSG_WAITALL);
    if (iResult == SOCKET_ERROR) {
        auto exc = WinapiException("recv", WinapiErrornoMethod::wsaError);
        if (exc.getErrorno() == WSAECONNRESET) {
            throw ClientException::ClientDisconnected;
        } else {
            throw exc;
        }
    }
    if (iResult == 0 || iResult < recvbuflen) {
        throw ClientException::ClientDisconnected;
    }

}

ClientSocket::~ClientSocket() {
    // blank
}


void ClientSocket::sendall(const char* sendbuf, int len) {
    while (len > 0) {
        int iResult = send(m_sock.getSocket(), sendbuf, len, 0);
        if (iResult == SOCKET_ERROR) {
            auto exc = WinapiException("send", WinapiErrornoMethod::wsaError);
            if (exc.getErrorno() == WSAECONNRESET) {
                throw ClientException::ClientDisconnected;
            } else {
                throw exc;
            }
        }
        if (iResult == 0) {
            throw ClientException::ClientDisconnected;
        }
        len -= iResult;
        sendbuf += iResult;
    }
    
}

void ClientSocket::sendall(const std::string& s) {
    sendall(s.c_str(), static_cast<int>(s.size()));
}

int bytesToInt(const char* buff, int len) {
    // convert a byte array to an integer (little-endian)
    int ans = 0;
    memcpy(&ans, buff, len);
    return ans;
}

void intToBytes(int value, char* buff, int len) {
    // convert an integer to a byte array (little-endian)
    memcpy(buff, &value, len);
}

std::string ClientSocket::recvMsg() {
    // recv a msg - recv size and than the actual message.
    char sizeBuff[MSG_SIZE_SIZE];
    recvall(sizeBuff, MSG_SIZE_SIZE);
    int size = bytesToInt(sizeBuff, MSG_SIZE_SIZE);
    char* msg = new char[size+1];
    recvall(msg, size);
    msg[size] = '\0';
    std::string ans = msg;
    delete[] msg;
    return ans;
}

void ClientSocket::sendMsg(const char* sendbuf, int len) {
    // send a msg - send size and than the actual message
    if (len > (1 << (MSG_SIZE_SIZE*8))) {
        throw std::exception("Invalid message size!");
    }
    char sizeBuff[MSG_SIZE_SIZE];
    intToBytes(len, sizeBuff, MSG_SIZE_SIZE);
    sendall(sizeBuff, MSG_SIZE_SIZE);
    sendall(sendbuf, len);
}

void ClientSocket::sendMsg(const std::string& s) {
    // send a msg - send size and than the actual message
    sendMsg(s.c_str(), static_cast<int>(s.size()));
}


std::string ClientSocket::recvFile() {
    std::string fileName = recvMsg();
    
    char sizeBuff[FILE_SIZE_SIZE];
    recvall(sizeBuff, FILE_SIZE_SIZE);
    int size = bytesToInt(sizeBuff, FILE_SIZE_SIZE);
    // receiving all and than writing might be to much
    char* cfileName = new char[fileName.length() + 1];
    strcpy_s(cfileName, fileName.length() + 1, fileName.c_str());
    Handle file = CreateFileA(
        cfileName,
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, 
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    delete[] cfileName;
    if (file.getHandle() == INVALID_HANDLE_VALUE) {
        throw WinapiException("CreateFileA",
                              WinapiErrornoMethod::standardError);
    }
    Handle fileMap = CreateFileMappingA(
        file.getHandle(),
        NULL, 
        PAGE_READWRITE,
        0,
        size,
        NULL
    );

    if (fileMap.getHandle() == NULL) {
        throw WinapiException("CreateFileMappingA",
                              WinapiErrornoMethod::standardError);
    }

    PCHAR buf = static_cast<PCHAR>(MapViewOfFile(
        fileMap.getHandle(),
        FILE_MAP_ALL_ACCESS, // readwrite
        0,
        0,
        size
        ));

    if (buf == NULL) {
        throw WinapiException("MapViewOfFile",
                              WinapiErrornoMethod::standardError);
    }

    recvall(buf, size);
    return fileName;
}



void ClientSocket::sendFile(const std::string& fileName) {
    
    char* cfileName = new char[fileName.length() + 1];
    strcpy_s(cfileName, fileName.length() + 1, fileName.c_str());
    Handle file = CreateFileA(
        cfileName,
        GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, 
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    delete[] cfileName;
    if (file.getHandle() == INVALID_HANDLE_VALUE) {
        throw WinapiException("CreateFileA",
                              WinapiErrornoMethod::standardError);
    }

    DWORD fileSize = GetFileSize(file.getHandle(), NULL);
    

    Handle fileMap = CreateFileMappingA(
        file.getHandle(),
        NULL, 
        PAGE_READONLY,
        0,
        fileSize,
        NULL
    );

    if (fileMap.getHandle() == NULL) {
        throw WinapiException("CreateFileMappingA",
                              WinapiErrornoMethod::standardError);
    }

    PCHAR buf = static_cast<PCHAR>(MapViewOfFile(
        fileMap.getHandle(),
        FILE_MAP_READ, 
        0,
        0,
        fileSize
        ));
    if (buf == NULL) {
        throw WinapiException("MapViewOfFile",
                              WinapiErrornoMethod::standardError);
    }

    sendMsg(fileName.substr(fileName.find_last_of('\\') + 1));
    char sizeBuff[FILE_SIZE_SIZE];
    intToBytes(fileSize, sizeBuff, FILE_SIZE_SIZE);
    sendall(sizeBuff, FILE_SIZE_SIZE);
    sendall(buf, fileSize);
}

void ClientSocket::sendHash(const std::string& fileName) {
    MSIFILEHASHINFO hash;
    getFileHash(fileName, &hash);
    sendall(reinterpret_cast<char*>(&hash), sizeof(MSIFILEHASHINFO));
}

void ClientSocket::validateFileHash(const std::string& fileName) {
    MSIFILEHASHINFO hash1;
    getFileHash(fileName, &hash1);
    MSIFILEHASHINFO hash2;
    recvall(reinterpret_cast<char*>(&hash2), sizeof(MSIFILEHASHINFO));

    if (!isHashEqual(&hash1, &hash2)) {
        throw ClientException::hashDidNotMatch;
    }
}

