#pragma once

// STL includes
#include <cstdint>
#include <string>
#include <string_view>

// Project includes
#include "HostState.hpp"
#include "PairStatus.hpp"
#include "../Utilities/Version.hpp"

// Forward declarations
namespace tinyxml2
{
    class XMLElement;
}

namespace MoonlightOBS
{
    /**
     * @brief Represents the settings for a GameStream host.
     * 
     */
    class HostSettings
    {
    public:
        /**
         * @brief Construct a new Host Settings from a response from the GameStream host.
         * @exception std::invalid_argument If the raw response is empty, invalid, 
         *            or is unable to be parsed.
         * 
         * @param rawResponse The XML response from the GameStream host.
         */
        HostSettings(std::string_view rawResponse);

        /**
         * @brief Get the Hostname of the GameStream host.
         * 
         * @return std::string The hostname of the GameStream host.
         */
        inline std::string GetHostname() const
        {
            return m_hostname;
        }

        /**
         * @brief Get the Version of the GameStream host.
         * 
         * @return Version The version of the GameStream host.
         */
        inline Version GetAppVersion() const
        {
            return m_appVersion;
        }

        /**
         * @brief Get the version of GeForce Experience of the host.
         * @note This is a legacy field, as GeForce Experience is no longer used by Sunshine.
         * 
         * @return Version The version of the GeForce Experience of the host.
         */
        inline Version GetGFEVersion() const
        {
            return m_gfeVersion;
        }

        /**
         * @brief Get the Unique ID of the GameStream host.
         * 
         * @return std::string The unique ID of the GameStream host.
         */
        inline std::string GetUniqueID() const
        {
            return m_uniqueID;
        }

        /**
         * @brief Gets the HTTPS port of the GameStream host.
         * 
         * @return uint16_t The HTTPS port of the GameStream host.
         */
        inline uint16_t GetHTTPSPort() const
        {
            return m_httpsPort;
        }

        /**
         * @brief Gets the external port of the GameStream host.
         * 
         * @return uint16_t The external port of the GameStream host.
         */
        inline uint16_t GetExternalPort() const
        {
            return m_externalPort;
        }
        
        /**
         * @brief Gets the maximum luma pixels for H.264 encoding.
         * 
         * @return uint64_t The maximum luma pixels for H.264 encoding.
         */
        inline uint64_t GetMaxLumaPixelsHEVC() const
        {
            return m_maxLumaPixelsHEVC;
        }

        /**
         * @brief Gets the MAC address of the GameStream host.
         * 
         * @return std::string The MAC address of the GameStream host.
         */
        inline std::string GetMacAddress() const
        {
            return m_macAddress;
        }

        /**
         * @brief Gets the server codec mode support flag of the GameStream host.
         * 
         * @return int The server codec mode support flag of the GameStream host.
         */
        inline int GetServerCodecModeSupport() const
        {
            return m_serverCodecModeSupport;
        }

        /**
         * @brief Gets the local IP address of the GameStream host.
         * 
         * @return std::string The local IP address of the GameStream host.
         */
        inline std::string GetLocalIP() const
        {
            return m_localIP;
        }

        /**
         * @brief Gets the pair status of the GameStream host.
         * 
         * @return PairStatus The pair status of the GameStream host.
         */
        inline PairStatus GetPairStatus() const
        {
            return m_pairStatus;
        }

        /**
         * @brief Gets the code of the current game running on the GameStream host.
         * 
         * @return int The code of the current game running on the GameStream host.
         */
        inline int GetCurrentGame() const
        {
            return m_currentGame;
        }

        /**
         * @brief Gets the current streaming state of the GameStream host.
         * 
         * @return HostState The current streaming state of the GameStream host.
         */
        inline HostState GetHostState() const
        {
            return m_hostState;
        }

    private:
        // The hostname of the GameStream host
        // (By default, this is the hostname of the host
        // but it can be overridden by the user within Sunshine's settings)
        std::string m_hostname;

        // The unique ID of the GameStream host
        std::string m_uniqueID;
        
        // The MAC address of the GameStream host
        std::string m_macAddress;

        // The local IP address of the GameStream host
        std::string m_localIP;

        // The version of the Sunshine host
        Version m_appVersion;

        // The version of GeForce Experience of the host
        // (This is a legacy field, as GeForce Experience is no longer used by Sunshine)
        Version m_gfeVersion;

        // The maximum luma pixels for H.264 encoding
        uint64_t m_maxLumaPixelsHEVC;

        // The current game running on the GameStream host
        int m_currentGame;

        // The server codec mode support of the GameStream host
        int m_serverCodecModeSupport;

        // The pair status of the GameStream host
        PairStatus m_pairStatus;

        // The current streaming state of the GameStream host
        HostState m_hostState;

        // The HTTPS port of the GameStream host
        uint16_t m_httpsPort;

        // The external port of the GameStream host
        uint16_t m_externalPort;

        // Helper function to convert a an XML element to a string
        static std::string XMLElementToString(const tinyxml2::XMLElement* element);
        // Helper function to parse an XML element to a version number
        static Version XMLELementToVersion(const tinyxml2::XMLElement* element);
        // Helper function to parse an XML element to a 64 bit unsigned integer
        static uint64_t XMLElementToUInt64(const tinyxml2::XMLElement* element);
        // Helper function to parse an XML element to an integer
        static int XMLElementToInt(const tinyxml2::XMLElement* element);
        // Helper function to prase an XML element to a PairStatus enumeration
        static PairStatus XMLElementToPairStatus(const tinyxml2::XMLElement* element);
        // Helper function to parse an XML element to a HostState enumeration
        static HostState XMLElementToHostState(const tinyxml2::XMLElement* element);
    };
} // namespace MoonlightOBS
