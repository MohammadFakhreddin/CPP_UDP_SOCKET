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

SocketHandle CreateSocket(U16 const port) {
    auto const handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(handle <= 0) {
        printf( "Failed to create socket\n" );
        return {};
    }
    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( static_cast<unsigned short>(port));
    if (bind(handle, 
            reinterpret_cast<const sockaddr *>(&address), 
            sizeof(sockaddr_in)) < 0) {
        printf( "Failed to bind socket\n" );
        return {};
    }
#ifdef __PLATFORM_WIN__
    DWORD nonBlocking = 1;
    if ( ioctlsocket( handle, 
                      FIONBIO, 
                      &nonBlocking ) != 0 ) {
        printf( "failed to set non-blocking\n" );
        return {};
    }
#else
    int nonBlocking = 1;
    if ( fcntl( handle, 
                F_SETFL, 
                O_NONBLOCK, 
                nonBlocking ) == -1 ) {
        printf( "failed to set non-blocking\n" );
        return false;
    }
#endif
    return {handle, port};
}

SocketAddress CreateAddress(SocketHandle const & socket, IPAddress const & ip_address) {
    if(socket.valid()) {
        U32 const address = ( ip_address.a << 24 ) | 
                            ( ip_address.b << 16 ) | 
                            ( ip_address.c << 8  ) | 
                            ( ip_address.d );
        sockaddr_in socket_address {};
        socket_address.sin_family = AF_INET;
        socket_address.sin_addr.s_addr = htonl(address);
        socket_address.sin_port = htons(socket.port);
        return {socket_address, true};
    }
    return {};
}

bool SendPacked(
    SocketHandle const & socket,
    Blob const & packet,
    SocketAddress const & address
) {
    bool ret = false;
    if(socket.valid() && address.valid) {
        auto const sent_bytes = sendto(
            socket.handle,
            reinterpret_cast<char const *>(packet.ptr), 
            static_cast<I32>(packet.len), 
            0,
            (struct sockaddr *)&address.address, 
            sizeof(sockaddr) 
        );
        if (sent_bytes == packet.len) {
            ret = true;
        } else {
            printf( "failed to send packet\n" );
        }
    }
    return ret;
}


}
