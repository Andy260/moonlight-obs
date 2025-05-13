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
    class Host
    {
    public:
        /**
         * @brief Construct a new Host object.
         * 
         * @param address The address of the GameStream host.
         * @param name The name of the GameStream host.
         */
        inline Host(Address address, std::string_view name) : 
            m_address(address),
            m_name(name) {}

        /**
         * @brief Gets an empty Host object.
         * 
         * @return Host An empty Host object.
         */
        inline static Host GetEmpty()
        {
            return Host(Address("", 0), "");
        }

        /**
         * @brief Get the Address of the GameStream host.
         * 
         * @return Address The address of the GameStream host.
         */
        inline Address GetAddress() const
        {
            return m_address;
        }

        /**
         * @brief Get the Name GameStream host.
         * 
         * @return std::string The name of the GameStream host.
         */
        inline std::string GetName() const
        {
            return m_name;
        }

        /**
         * @brief Compares two Host objects for equality.
         * 
         * @param other The other Host object to compare with.
         * @return true The two Host objects are equal
         *         -or-
         *         false The two Host objects are not equal.
         */
        bool operator==(const Host& other) const
        {
            return m_address == other.m_address && m_name == other.m_name;
        }

        /**
         * @brief Compares two Host objects for inequality.
         * 
         * @param other The other Host object to compare with.
         * @return true The two Host objects are not equal
         *         -or-
         *         false The two Host objects are equal.
         */
        bool operator!=(const Host& other) const
        {
            return !(*this == other);
        }
        
    private:
        // The address of the host
        Address m_address;
        // The name of the host
        std::string m_name;
    };
} // MoonLightOBS
