#include <iostream>

#include "FoundationSocketBackend.hpp"
#include "BedrockAssert.hpp"

int main(){
    auto const init_socket_result = MFA::SocketBackend::Init();
    MFA_ASSERT(true == init_socket_result); MFA_CONSUME_VAR(init_socket_result);
    auto const socket = MFA::SocketBackend::CreateSocket(3000);
    MFA_ASSERT(socket.valid()); MFA_CONSUME_VAR(socket);
    {// Sending data
        auto const address = MFA::SocketBackend::CreateAddress(socket, {127,0,0,1});
        char message[100] = "Hello from other side\0";
        auto const send_packet_result = MFA::SocketBackend::SendPacked(
            socket, 
            {message, strlen(message)}, 
            address
        ); MFA_ASSERT(true == send_packet_result); MFA_CONSUME_VAR(send_packet_result);
    }
    {// Receiving data
        char received_message[256];
        MFA::SocketBackend::IPAddress received_data_address {};
        MFA::SocketBackend::Port received_data_port;
        auto const received_bytes = MFA::SocketBackend::TryReceiveData(
            socket, 
            {received_message, 256 - 1}, 
            received_data_address, 
            received_data_port
        ); MFA_ASSERT(received_bytes > 0);
        received_message[received_bytes] = '\0';
        std::cout << "Received message is " << received_message << std::endl;
    }
    MFA::SocketBackend::DestroySocket(socket);
    MFA::SocketBackend::Shutdown();
    return 0;
}