
#ifndef FOUNDATION_SOCKET_BACKEND
#define FOUNDATION_SOCKET_BACKEND

#include "BedrockPlatforms.hpp"

#ifdef __PLATFORM_WIN__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

namespace MFA::SocketBackend {

bool Init();

void Shutdown();


}

#endif
