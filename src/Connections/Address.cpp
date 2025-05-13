#include "Address.hpp"

// STL includes
#include <stdexcept>

using namespace MoonlightOBS;

Address::Address(std::string_view address, uint16_t port)
    : m_address(address), m_port(port)
{
    // Ensure the port number is valid
    if (port > 65535)
    {
        throw std::out_of_range("Port number must be between 0 and 65535");
    }
}

void Address::SetPort(uint16_t port)
{
    // Ensure the port number is valid
    if (port > 65535)
    {
        throw std::out_of_range("Port number must be between 0 and 65535");
    }
    
    m_port = port;
}
