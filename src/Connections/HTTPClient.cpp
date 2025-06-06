#include "HTTPClient.hpp"

// STL includes
#include <stdexcept>
#include <string>

// libcurl includes
#include <curl/curl.h>

// Workaround for Windows min/max macros conflicting with std::min/std::max.
// This should be placed before including headers that use std::min or std::max,
// or headers that might themselves include Windows headers without NOMINMAX.
// 
// This seems to be caused by the libcurl headers including Windows headers,
// which define min and max as macros, causing conflicts with the C++ standard library.
#if defined(_WIN32) || defined(_WIN64)
  // If min/max are defined as macros, undefine them.
  #ifdef min
    #undef min
  #endif
  #ifdef max
    #undef max
  #endif
  // Defining NOMINMAX before including windows.h is another common solution,
  // but undefining is more direct if windows.h has already been included transitively.
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif

// OBS Studio includes
#include <util/base.h>

// Project includes
#include "../plugin-support.h"
#include "Address.hpp"
#include "HostSettings.hpp"

using namespace MoonlightOBS;

HTTPClient::HTTPClient(const Address& address)
    : m_curl(static_cast<void*>(curl_easy_init())), m_address(address)
{
    // Check if libcurl was initialized successfully
    if (m_curl == nullptr)
    {
        throw std::runtime_error("Failed to initialize libcurl");
    }
}

HTTPClient::~HTTPClient()
{
    // Clean up the libcurl handle
    if (m_curl != nullptr)
    {
        curl_easy_cleanup(static_cast<CURL*>(m_curl));
        m_curl = nullptr;
    }
}

HostSettings HTTPClient::GetServerInfo() const
{
    CURL* curl = static_cast<CURL*>(m_curl);

    // Calculate the URL for the request
    std::string url = "http://" + m_address.GetString() + "/serverinfo";

    // Set the URL for the request
    CURLcode statusCode = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (statusCode != CURLE_OK)
    {
        throw std::runtime_error("Failed to set URL: " + std::string(curl_easy_strerror(statusCode)));
    }

    // Set the write function to capture the response
    statusCode = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriteCallback);
    if (statusCode != CURLE_OK)
    {
        throw std::runtime_error("Failed to set write function: " + std::string(curl_easy_strerror(statusCode)));
    }

    // Set the write data to capture the response
    ResponseData readBuffer;
    statusCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    if (statusCode != CURLE_OK)
    {
        throw std::runtime_error("Failed to set write data: " + std::string(curl_easy_strerror(statusCode)));
    }

    // Perform the request
    statusCode = curl_easy_perform(curl);

    // Check if the request was successful
    if (statusCode != CURLE_OK)
    {
        throw std::runtime_error("Failed to perform request: " + std::string(curl_easy_strerror(statusCode)));
    }
    
    // Parse the response into HostSettings and return it
    return HostSettings(readBuffer.data);
}

size_t HTTPClient::CURLWriteCallback(char* data, size_t size, size_t nmemb, void *clientp)
{
    size_t newLength = size * nmemb;
    ResponseData* responseData = static_cast<ResponseData*>(clientp);

    try
    {
        responseData->data.append(data, newLength);
    }
    catch (const std::exception& exception)
    {
        // Memory allocation failed, log the error
        obs_log(LOG_ERROR, "Failed to append data while performing a HTTP request: %s", exception.what());
    }
    
    return newLength;
}
