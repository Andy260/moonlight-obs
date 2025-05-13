#pragma once

// STL includes
#include <ostream>
#include <string>
#include <cstdint>

namespace MoonlightOBS
{
    /**
     * @brief Represents an address for a connection.
     * 
     */
    class Address
    {
    public:
        /**
         * @brief Construct a new Address object
         * 
         * @param address The IP address or hostname of the connection.
         * @param port The port number of the connection. (Must be between 0 and 65535)
         * 
         * @exception std::out_of_range If the port number is not between 0 and 65535.
         */
        Address(std::string_view address, uint16_t port);

        /**
         * @brief Get the IP address or hostname of the connection.
         * 
         * @return std::string_view The address of the connection.
         */
        inline std::string_view GetAddress() const
        {
            return m_address;
        }

        /**
         * @brief Set the IP address or hostname of the connection.
         * 
         * @param address The address of the connection.
         */
        inline void SetAddress(std::string_view address)
        {
            m_address = address;
        }

        /**
         * @brief Gets the port number of the connection.
         * 
         * @return uint16_t The port number of the connection.
         */
        inline uint16_t GetPort() const
        {
            return m_port;
        }

        /**
         * @brief Sets the port number of the connection.
         * 
         * @param port The port number of the connection. (Must be between 0 and 65535)
         * 
         * @exception std::out_of_range If the port number is not between 0 and 65535.
         */
        void SetPort(uint16_t port);

        /**
         * @brief Converts the Address object to a string representation.
         * 
         * @return std::string The string representation of the Address object.
         */
        inline std::string GetString() const
        {
            return m_address + ":" + std::to_string(m_port);
        }

        /**
         * @brief Converts the Address object to a string representation.
         */
        inline friend std::ostream& operator<<(std::ostream& os, const Address& address)
        {
            os << address.m_address << ":" << address.m_port;
            return os;
        }

        /**
         * @brief Compares two Address objects for equality.
         * 
         * @param other The other Address object to compare with.
         * @return true If the addresses are equal.
         * @return false If the addresses are not equal.
         */
        inline bool operator==(const Address& other) const
        {
            return m_address == other.m_address && m_port == other.m_port;
        }

        /**
         * @brief Compares two Address objects for inequality.
         * 
         * @param other The other Address object to compare with.
         * @return true If the addresses are not equal.
         * @return false If the addresses are equal.
         */
        inline bool operator!=(const Address& other) const
        {
            return !(*this == other);
        }

    private:
        // Hostname or IP address of the connection
        std::string m_address;
        // Port number for the connection
        uint16_t m_port;
    };
} // namespace MoonlightOBS
