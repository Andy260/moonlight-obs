#pragma once

// Project includes
#include "../Connections/Address.hpp"

namespace MoonlightOBS
{
    // Forward declarations
    class HostSettings;

    /**
     * @brief Provides a simple HTTP client for making requests 
     *        to GameStream hosts via HTTP(S).
     * 
     */
    class HTTPClient
    {
    public:
        /**
         * @brief Construct a new HTTPClient object.
         * 
         * @param address The address of the GameStream host to connect to.
         */
        HTTPClient(const Address& address);

        /**
         * @brief Destroy the HTTPClient object.
         */
        ~HTTPClient();

        /**
         * @brief Gets the settings of the GameStream host.
         * @exception std::runtime_error If the request fails or the response is invalid.
         * 
         * @return HostSettings The settings of the GameStream host.
         */
        HostSettings GetServerInfo() const;

    private:
        struct ResponseData
        {
            std::string data; // Holds the response data
        };
        
        // libcurl handle
        void* m_curl;

        // Address of the GameStream host
        Address m_address;

        // Callback function for writing data called by libcurl
        static size_t CURLWriteCallback(char *data, size_t size, size_t nmemb, void *clientp);
    };
} // namespace MoonlightOBS
