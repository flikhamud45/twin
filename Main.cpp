#include "Main.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


constexpr char MUTEX_NAME[] = "technai_mutex";
constexpr char RUN_REG[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr WCHAR PROGRAM_NAME[] = L"Technai";
constexpr WCHAR PROGRAM_PATH[] = L"C:\\Users\\User\\source\\repos\\twin\\x64\\Debug\\twin.exe";
constexpr char DEFAULT_MSG[] = "MANAGMENT PROGRAM IS UP";
constexpr char DEFAULT_TITLE[] = "MANAGMENT PROGRAM";



constexpr char ERROR_MSG[] = "Unknown Command";
constexpr char PING_COMMAND[] = "ping";
constexpr char PONG_COMMAND[] = "pong";

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



Mutex* ensureOneProgram() {
    // ensure that there is only one program running and return a locked mutex. mutex object must be freed
    Mutex* mutex = new Mutex(MUTEX_NAME);

    if (mutex->getHandle() == NULL) {
        throw WinapiException("CreateMutexA", WinapiErrornoMethod::standardError);
    }

    DWORD waitStatus = WaitForSingleObject(mutex->getHandle(), 0);
    if (waitStatus == WAIT_OBJECT_0 || waitStatus == WAIT_ABANDONED) {
        return mutex;
    } else if (waitStatus == WAIT_TIMEOUT) {
        std::cout << "program is already running...\n";
        delete mutex;
        exit(1);
    } else if (waitStatus == WAIT_FAILED){
        delete mutex;
        throw WinapiException("WaitForSingleObject", WinapiErrornoMethod::standardError);
    } else {
        delete mutex;
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

    status = RegSetKeyValueW(hkey, NULL, PROGRAM_NAME, REG_SZ, PROGRAM_PATH,
                             static_cast<DWORD>(wcslen(PROGRAM_PATH)) +
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

void handleMsg(ClientSocket& client, const std::string& msg) {
    openMessageBox(msg.c_str());
    if (msg == PING_COMMAND)
        client.sendMsg(PONG_COMMAND);
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
            default:
                break;
            }
        }
    }
}

void startServer()
{
    
    ServerSocket serverSock;
    serverSock.bind();
    serverSock.listen();
    while (TRUE) {
        ClientSocket client = serverSock.accept();
        handleClient(client);
    }

}

int main() {
    Mutex* m = NULL;
    try {
        m = ensureOneProgram();
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
    if (m != NULL) {
        delete m;
    }
}

