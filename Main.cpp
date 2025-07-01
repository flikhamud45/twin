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

WinapiException::WinapiException(const char* lastFunc, WinapiErrorType error) : m_lastFunc(lastFunc) {
    switch (error) {
    case WinapiErrorType::standardError:
        m_errorno = GetLastError();
        break;
    case WinapiErrorType::wsaError:
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
    // ensure that there is only one program running and return a locked mutex.
    HANDLE mutex = CreateMutexA(NULL, TRUE, MUTEX_NAME);

    if (mutex == NULL) {
        throw WinapiException("CreateMutexA", WinapiErrorType::standardError);
    }

    DWORD waitStatus = WaitForSingleObject(mutex, 0);
    if (waitStatus == WAIT_OBJECT_0 || waitStatus == WAIT_ABANDONED) {
        return mutex;
    } else if (waitStatus == WAIT_TIMEOUT) {
        std::cout << "program is already running...\n";
        exit(1);
    } else if (waitStatus == WAIT_FAILED){
        throw WinapiException("WaitForSingleObject", WinapiErrorType::standardError);
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

        throw WinapiException("MessageBoxA", WinapiErrorType::standardError);
    }
}

void handleClient(ClientSocket client) {
    try {
        std::string msg = client.recvMsg();
        if (msg ==PING_COMMAND)
            client.sendMsg(PONG_COMMAND);
        else{
            throw ClientException::invalidMsg;
        }
    }
    catch (ClientException e) {
        switch (e) {
        case ClientException::invalidMsg:
            client.sendMsg(ERROR_MSG);
        }
    }
}

void startServer()
{
    
    ServerSocket serverSock;
    serverSock.bind();
    serverSock.listen();
    ClientSocket client = serverSock.accept();
}

int main() {
    try {
        Mutex m = ensureOneProgram();
        runOnStartUp();
        openMessageBox();
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

