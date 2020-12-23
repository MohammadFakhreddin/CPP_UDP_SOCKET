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

SocketHandle CreateSocket(Port const port) {
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

bool DestroySocket(SocketHandle const & socket) {
    bool ret = false;
    if(socket.valid()) {
#ifdef __PLATFORM_WIN__
        closesocket( socket.handle );
#else
    close( socket.handle );
#endif
        ret = true;
    }
    return ret;
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

U32 TryReceiveData(
    SocketHandle const & socket,
    Blob const & out_buffer,
    IPAddress & out_address,
    Port & out_port
) {
#ifdef __PLATFORM_WIN__
    typedef int socklen_t;
#endif
    sockaddr_in from;
    socklen_t fromLength = sizeof( from );
    auto const received_bytes = recvfrom(
        socket.handle, 
        reinterpret_cast<char *>(out_buffer.ptr), 
        static_cast<I32>(out_buffer.len), 
        0, 
        reinterpret_cast<sockaddr *>(&from), 
        &fromLength 
    );
    if(received_bytes > 0) {
        unsigned int from_address = ntohl( from.sin_addr.s_addr );
        out_address.a = static_cast<U8>(from_address >> 24);
        from_address = from_address & 000'111'111'111;
        out_address.b = static_cast<U8>(from_address >> 16);
        from_address = from_address & 000'000'111'111;
        out_address.c = static_cast<U8>(from_address >> 8);
        from_address = from_address & 000'000'000'111;
        out_address.d = static_cast<U8>(from_address); 
        out_port = ntohs( from.sin_port );
    }
    return received_bytes;
}


}
