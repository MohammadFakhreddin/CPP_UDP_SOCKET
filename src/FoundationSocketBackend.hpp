
#ifndef FOUNDATION_SOCKET_BACKEND
#define FOUNDATION_SOCKET_BACKEND

#include "BedrockCommon.hpp"
#include "BedrockPlatforms.hpp"

#ifdef __PLATFORM_WIN__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

namespace MFA::SocketBackend {

using Port = U32;

struct SocketHandle {
    [[nodiscard]]
    bool valid() const {
        return handle > 0;
    }
    SOCKET const handle {};
    Port const port;
};

bool Init();

void Shutdown();

SocketHandle CreateSocket(Port port);

struct IPAddress {
    U8 a,b,c,d;
};
struct SocketAddress {
    sockaddr_in const address {};
    bool const valid = false;
};
SocketAddress CreateAddress(
    SocketHandle const & socket, 
    IPAddress const & ip_address
);

// Packet size must as short as possible
bool SendPacked(
    SocketHandle const & socket,
    Blob const & packet,
    SocketAddress const & address
);

bool TryReceiveData(
    SocketHandle const & socket,
    Blob const & out_buffer,
    IPAddress & out_address,
    Port & out_port
);

}

#endif
