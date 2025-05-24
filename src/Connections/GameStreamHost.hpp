#pragma once

// STL includes
#include <string>

// Project includes
#include "Address.hpp"

namespace MoonlightOBS
{
    /**
     * @brief Represents a GameStream host.
     * 
     */
    class GameStreamHost
    {
    public:
        /**
         * @brief Construct a new GameStreamHost object.
         * 
         * @param hostname The hostname of the GameStream host.
         * @param ipv4Address The IPv4 address of the host.
         * @param ipv6Address The IPv6 address of the host.
         */
        inline GameStreamHost(std::string_view hostname, Address ipv4Address, Address ipv6Address) : 
            m_ipv4Address(ipv4Address),
            m_ipv6Address(ipv6Address),
            m_hostname(hostname) {}

        /**
         * @brief Gets an empty GameStreamHost object.
         * 
         * @return Host An empty GameStreamHost object.
         */
        inline static GameStreamHost GetEmpty()
        {
            return GameStreamHost("", Address::GetEmpty(), Address::GetEmpty());
        }

        /**
         * @brief Creates a GameStreamHost object from an IPv4 address and hostname.
         * 
         * @param hostname The hostname of the GameStream host.
         * @param ipv4Address The IPv4 address of the host.
         * @return GameStreamHost The created GameStreamHost object.
         */
        inline static GameStreamHost FromIPv4(std::string_view hostname, Address ipv4Address)
        {
            return GameStreamHost(hostname, ipv4Address, Address("", 0));
        }

        /**
         * @brief Creates a GameStreamHost object from an IPv6 address and hostname.
         * 
         * @param hostname The hostname of the GameStream host.
         * @param ipv6Address The IPv6 address of the host.
         * @return GameStreamHost The created GameStreamHost object.
         */
        inline static GameStreamHost FromIPv6(std::string_view hostname, Address ipv6Address)
        {
            return GameStreamHost(hostname, Address("", 0), ipv6Address);
        }
        
        /**
         * @brief Creates a GameStreamHost object from a hostname.
         * 
         * @param hostname The hostname of the GameStream host.
         * @return GameStreamHost The created GameStreamHost object.
         */
        inline static GameStreamHost FromHostname(std::string_view hostname)
        {
            return GameStreamHost(hostname, Address("", 0), Address("", 0));
        }

        /**
         * @brief Get the IPv4 Address of the GameStream host.
         * 
         * @return Address The IPv4 address of the GameStream host.
         */
        inline Address GetIPv4Address() const
        {
            return m_ipv4Address;
        }

        /**
         * @brief Sets the IPv4 Address of the GameStream host.
         * 
         * @param ipv4Address The IPv4 address of the GameStream host.
         */
        inline void SetIPv4Address(Address ipv4Address)
        {
            m_ipv4Address = ipv4Address;
        }

        /**
         * @brief Get the IPv6 Address of the GameStream host.
         * 
         * @return Address The IPv6 address of the GameStream host.
         */
        inline Address GetIPv6Address() const
        {
            return m_ipv6Address;
        }

        /**
         * @brief Sets the IPv6 Address of the GameStream host.
         * 
         * @param ipv6Address The IPv6 address of the GameStream host.
         */
        inline void SetIPv6Address(Address ipv6Address)
        {
            m_ipv6Address = ipv6Address;
        }

        /**
         * @brief Checks if this object has an IPv6 address.
         * 
         * @return true The object has an IPv6 address.
         * @return false The object does not have an IPv6 address.
         */
        inline bool HasIPv6Address() const
        {
            return m_ipv6Address.GetPortNumber() != 0;
        }

        /**
         * @brief Gets the hostname of the GameStream host.
         * 
         * @return std::string The hostname of the GameStream host.
         */
        inline std::string GetHostname() const
        {
            return m_hostname;
        }

        /**
         * @brief Sets the hostname of the GameStream host.
         * 
         * @param hostname The hostname of the GameStream host.
         */
        inline void SetHostname(std::string_view hostname)
        {
            m_hostname = hostname;
        }

        /**
         * @brief Checks if this object is valid.
         * 
         * @return true This object is valid.
         * @return false This object is not valid.
         */
        inline bool IsValid() const
        {
            return !m_hostname.empty() && (m_ipv4Address.GetPortNumber() != 0 
                || m_ipv6Address.GetPortNumber() != 0);
        }
        
        /**
         * @brief Compares two GameStreamHost objects for equality.
         * 
         * @param other The other GameStreamHost object to compare with.
         * @return true The two GameStreamHost objects are equal
         *         -or-
         *         false The two GameStreamHost objects are not equal.
         */
        bool operator==(const GameStreamHost& other) const
        {
            return m_hostname == other.m_hostname && m_ipv4Address == other.m_ipv4Address && 
                m_ipv6Address == other.m_ipv6Address;;
        }

        /**
         * @brief Compares two GameStreamHost objects for inequality.
         * 
         * @param other The other GameStreamHost object to compare with.
         * @return true The two GameStreamHost objects are not equal
         *         -or-
         *         false The two GameStreamHost objects are equal.
         */
        bool operator!=(const GameStreamHost& other) const
        {
            return !(*this == other);
        }
        
    private:
        // The IPv4 address of the host
        Address m_ipv4Address;
        // The IPv6 address of the host
        Address m_ipv6Address;
        // The hostname of the host
        std::string m_hostname;
    };
} // MoonLightOBS
