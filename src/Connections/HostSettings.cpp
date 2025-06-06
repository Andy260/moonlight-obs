#include "HostSettings.hpp"

// STL includes
#include <stdexcept>
#include <string>
#include <string_view>

// TinyXML includes
#include <tinyxml2.h>

// Project includes
#include "../Utilities/StringTools.hpp"
#include "../Utilities/Version.hpp"

using namespace MoonlightOBS;

HostSettings::HostSettings(std::string_view rawResponse)
    : m_appVersion(Version::GetUnknownVersion()), 
      m_gfeVersion(Version::GetUnknownVersion())
{
    // Check if the raw response is empty
    if (rawResponse.empty()) 
    {
        throw std::invalid_argument("Response cannot be empty");
    }

    // Parse the XML response using TinyXML2
    tinyxml2::XMLDocument serverInfoDoc;
    tinyxml2::XMLError errorStatus = serverInfoDoc.Parse(rawResponse.data(), rawResponse.size());

    // Check if the XML parsing was successful
    if (errorStatus != tinyxml2::XML_SUCCESS) 
    {
        throw std::invalid_argument("Invalid XML response: " + std::to_string(errorStatus));
    }

    // Get the root element of the XML document
    tinyxml2::XMLElement* rootElement = serverInfoDoc.RootElement();
    if (rootElement == nullptr)
    {
        throw std::invalid_argument("Invalid XML response: Does not contain a root element");
    }

    // Attempt to parse the XML document
    try
    {
        // Parse the "hostname" element
        m_hostname = XMLElementToString(rootElement->FirstChildElement("hostname"));
        // Parse the "appVersion" element
        m_appVersion = XMLELementToVersion(rootElement->FirstChildElement("appversion"));
        // Parse the "GfeVersion" element
        m_gfeVersion = XMLELementToVersion(rootElement->FirstChildElement("GfeVersion"));
        // Parse the "uniqueID" element
        m_uniqueID = XMLElementToString(rootElement->FirstChildElement("uniqueid"));
        // Parse the "HttpsPort" element
        m_httpsPort = static_cast<uint16_t>(XMLElementToInt(rootElement->FirstChildElement("HttpsPort")));
        // Parse the "ExternalPort" element
        m_externalPort = static_cast<uint16_t>(XMLElementToInt(rootElement->FirstChildElement("ExternalPort")));
        // Parse the "MaxLumaPixelsHEVC" element
        m_maxLumaPixelsHEVC = XMLElementToUInt64(rootElement->FirstChildElement("MaxLumaPixelsHEVC"));
        // Parse the "mac" element
        m_macAddress = XMLElementToString(rootElement->FirstChildElement("mac"));
        // Parse the "LocalIP" element
        m_localIP = XMLElementToString(rootElement->FirstChildElement("LocalIP"));
        // Parse the "ServerCodecModeSupport" element
        m_serverCodecModeSupport = XMLElementToInt(rootElement->FirstChildElement("ServerCodecModeSupport"));
        // Prase the "PairStatus" element
        m_pairStatus = XMLElementToPairStatus(rootElement->FirstChildElement("PairStatus"));
        // Parse the "currentgame" element
        m_currentGame = XMLElementToInt(rootElement->FirstChildElement("currentgame"));
        // Parse the "State" element
        m_hostState = XMLElementToHostState(rootElement->FirstChildElement("state"));
    }
    catch (const std::exception& exception)
    {
        // Error occurred while parsing the XML document
        throw std::invalid_argument("XML parsing error: " + std::string(exception.what()));
    }
}

std::string HostSettings::XMLElementToString(const tinyxml2::XMLElement* element)
{
    // Check if the element is null
    if (element == nullptr)
    {
        throw std::invalid_argument("Element cannot be null");
    }

    // Attempt to parse the text content of the element
    std::string value = element->GetText();
    // Check if the parsed value is empty
    if (value.empty())
    {
        // Get the name of the element
        std::string_view elementName = element->Name();

        throw std::invalid_argument("Element '" + std::string(elementName) + "' is empty");
    }

    return value;
}

Version HostSettings::XMLELementToVersion(const tinyxml2::XMLElement* element)
{
    // Check if the element is null
    if (element == nullptr)
    {
        throw std::invalid_argument("Element cannot be null");
    }

    // Attempt to parse the text content of the element
    std::string versionStr = element->GetText();
    // Check if the parsed value is empty
    if (versionStr.empty()) 
    {
        // Get the name of the element
        std::string_view elementName = element->Name();

        throw std::invalid_argument("Element '" + std::string(elementName) + "' is empty");
    }

    // Parse the version string
    Version version = Version::FromString(versionStr);
    
    // Return the parsed version
    return version;
}

uint64_t HostSettings::XMLElementToUInt64(const tinyxml2::XMLElement* element)
{
    // Check if the element is null
    if (element == nullptr)
    {
        throw std::invalid_argument("Element cannot be null");
    }

    // Attempt to parse the text content of the element
    uint64_t value = 0;
    tinyxml2::XMLError errorStatus = element->QueryUnsigned64Text(&value);

    // Check if the parsing was successful
    if (errorStatus != tinyxml2::XML_SUCCESS) 
    {
        // Get the name of the element
        std::string_view elementName = element->Name();

        throw std::invalid_argument("Element '" + std::string(elementName) + "' is not a valid unsigned integer: " + std::to_string(errorStatus));
    }

    return value;
}

int HostSettings::XMLElementToInt(const tinyxml2::XMLElement* element)
{
    // Check if the element is null
    if (element == nullptr)
    {
        throw std::invalid_argument("Element cannot be null");
    }

    // Attempt to parse the text content of the element
    int value = 0;
    tinyxml2::XMLError errorStatus = element->QueryIntText(&value);

    // Check if the parsing was successful
    if (errorStatus != tinyxml2::XML_SUCCESS) 
    {
        // Get the name of the element
        std::string_view elementName = element->Name();

        throw std::invalid_argument("Element '" + std::string(elementName) + "' is not a integer: " + std::to_string(errorStatus));
    }

    return value;
}

PairStatus HostSettings::XMLElementToPairStatus(const tinyxml2::XMLElement* element)
{
    // Check if the element is null
    if (element == nullptr)
    {
        throw std::invalid_argument("Element cannot be null");
    }

    // Attempt to parse the text content of the element
    int value = 0;
    tinyxml2::XMLError errorStatus = element->QueryIntText(&value);

    // Check if the parsing was successful
    if (errorStatus != tinyxml2::XML_SUCCESS) 
    {
        // Get the name of the element
        std::string_view elementName = element->Name();

        throw std::invalid_argument("Element '" + std::string(elementName) + "' is not a integer: " + std::to_string(errorStatus));
    }

    // Return the PairStatus based on the parsed value
    switch (value)
    {
        case 0:
            return PairStatus::Unpaired;
        case 1:
            return PairStatus::Paired;
        default:
            throw std::invalid_argument("Element '" + std::string(element->Name()) + "' has an unknown PairStatus value: " + std::to_string(value));
    }
}

HostState HostSettings::XMLElementToHostState(const tinyxml2::XMLElement* element)
{
    // Check if the element is null
    if (element == nullptr)
    {
        throw std::invalid_argument("Element cannot be null");
    }

    // Attempt to parse the text content of the element
    std::string value = element->GetText();
    // Check if the parsed value is empty
    if (value.empty())
    {
        // Get the name of the element
        std::string_view elementName = element->Name();

        throw std::invalid_argument("Element '" + std::string(elementName) + "' is empty");
    }
    
    // Return the HostState based on the parsed value
    if (value == "SUNSHINE_SERVER_FREE")
    {
        return HostState::SERVER_FREE;
    }
    else if (value == "SUNSHINE_SERVER_BUSY")
    {
        return HostState::SERVER_BUSY;
    }
    else
    {
        throw std::invalid_argument("Element '" + std::string(element->Name()) + "' has an unknown value: " + value);
    }
}
