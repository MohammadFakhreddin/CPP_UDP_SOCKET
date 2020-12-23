#include "FoundationSocketBackend.hpp"

namespace MFA::SocketBackend {

bool Init() {
#ifdef __PLATFORM_WIN__
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
#else
    return true;
#endif
}

void Shutdown() {
#ifdef __PLATFORM_WIN__
    WSACleanup();
#endif
}


}
