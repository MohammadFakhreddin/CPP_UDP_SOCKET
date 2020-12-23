#include <iostream>

#include "FoundationSocketBackend.hpp"
#include "BedrockAssert.hpp"

int main(){
    std::cout << "Sup" << std::endl;
    auto const init_socket_result = MFA::SocketBackend::Init();
    MFA_ASSERT(true == init_socket_result); MFA_CONSUME_VAR(init_socket_result);
    MFA::SocketBackend::Shutdown();
    return 0;
}