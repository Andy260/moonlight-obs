#include "mDNSRecordExtractor.hpp"

// STL includes
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

// mdns includes
#include <mdns.h>

// Project includes
#include "../Connections/Address.hpp"

using namespace MoonlightOBS;

mDNSRecordExtractor::mDNSRecordExtractor(std::string service_filter, int entryType_filterMask) 
    : m_responsesHandled(0), m_entryType_filterMask(entryType_filterMask), 
      m_service_filter(service_filter) {}

mDNSRecordExtractor mDNSRecordExtractor::Extract(int socket, int queryID_filter, std::string service_filter,
    int entryType_filterMask)
{
    // Ensure the socket is valid
    if (socket < 0)
    {
        throw std::invalid_argument("Invalid socket.");
    }

    // Create an instance of mDNSRecordExtractor to handle the records
    mDNSRecordExtractor extractor(service_filter, entryType_filterMask);

    // Handle the records from the mDNS query
    std::array<char, 512> responseBuffer;
    size_t responsesHandled = mdns_query_recv(socket, responseBuffer.data(), sizeof(char) * responseBuffer.size(), 
        &mDNSRecordExtractor::OnCallback, &extractor, queryID_filter);
    // Update the number of responses handled
    extractor.m_responsesHandled = responsesHandled;

    return extractor;
}

int mDNSRecordExtractor::OnCallback(int sock, const struct sockaddr* from, size_t addrlen,
                                    mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype,
                                    uint16_t rclass, uint32_t ttl, const void* data, size_t size,
                                    size_t name_offset, size_t name_length, size_t record_offset,
                                    size_t record_length, void* user_data)
{
    // Get a pointer to the mDNSRecordExtractor object
    mDNSRecordExtractor* extractor = static_cast<mDNSRecordExtractor*>(user_data);
    // Check if the pointer cast failed
    if (extractor == nullptr)
    {
        throw std::runtime_error("User data is null.");
    }

    // Check if the entry type is in the filter mask
    if ((static_cast<int>(entry) & extractor->m_entryType_filterMask) == 0)
    {
        // Ignore this record type
        return 0;
    }

    // Parse the name of the record
    std::string recordName = ExtractString_mDNS(data, size, name_offset);
    if (!extractor->m_service_filter.empty() && recordName != extractor->m_service_filter)
    {
        // Ignore records that aren't of the specified service type
        return 0;
    }

    // Parse the record
    mdns_record_type_t recordType = static_cast<mdns_record_type_t>(rtype);
    switch (recordType)
    {
        case MDNS_RECORDTYPE_IGNORE:
            // Ignore this record type
            break;

        // A record - IPv4 Address
        case MDNS_RECORDTYPE_A:
        {
            // Parse the A record
            sockaddr_in socketAddress;
            mdns_record_parse_a(data, size, record_offset, record_length, &socketAddress);

            // Convert the parsed IPv4 address to a string
            Address ipv4Address = SockaddrToAddress(socketAddress, sizeof(socketAddress));
            // Store the parsed IPv4 address
            extractor->m_ipv4Records.push_back(ipv4Address);
            break;
        }

        // PTR record - Domain Name pointer
        case MDNS_RECORDTYPE_PTR:
        {
            // Parse the PTR record
            std::array<char, 256> ptrNameBuffer;
            mdns_string_t ptrString_mdns = mdns_record_parse_ptr(data, size, record_offset, record_length, 
                ptrNameBuffer.data(), sizeof(char) * ptrNameBuffer.size());
            
            // Convert the parsed mdns string to a C++ string
            std::string ptrRecord(ptrString_mdns.str, ptrString_mdns.length);
            
            // Store the parsed PTR record
            extractor->m_ptrRecords.push_back(ptrRecord);
            break;
        }
        
        // TXT record - Arbitrary text string
        case MDNS_RECORDTYPE_TXT:
        {
            // Offset within the TXT record's RDATA
            size_t txtItem_offset = 0;
            // Buffer for a single "key=value" string
            std::array<mdns_record_txt_t, 32> txtItemBuffer;

            // Parse the TXT record
            size_t txtItemCount = mdns_record_parse_txt(data, size, record_offset, 
                record_length, txtItemBuffer.data(), txtItemBuffer.size());

            // Iterate over the parsed TXT records
            for (size_t i = 0; i < txtItemCount; ++i)
            {
                const mdns_record_txt_t& recordItem = txtItemBuffer[i];

                // Convert the parsed TXT record to a C++ string pair
                std::string key(recordItem.key.str, recordItem.key.length);
                std::string value(recordItem.value.str, recordItem.value.length);

                // Store the parsed TXT record
                extractor->m_txtRecords.emplace_back(key, value);
            }

            break;
        }

        // AAAA record - IPv6 Address [Thomson]
        case MDNS_RECORDTYPE_AAAA:
        {
            // Parse the AAAA record
            sockaddr_in6 socketAddress;
            mdns_record_parse_aaaa(data, size, record_offset, record_length, &socketAddress);

            // Convert the parsed IPv6 address to a string
            Address ipv6Address = SockaddrToAddress(socketAddress, sizeof(socketAddress));
            // Store the parsed IPv6 address
            extractor->m_ipv6Records.push_back(ipv6Address);
            break;
        }

        // SVR record - Server Selection [RFC2782]
        case MDNS_RECORDTYPE_SRV:
        {
            // Allocate the data for the SRV record
            std::array<char, 256> srvBuffer;

            // Parse the SRV record
            mdns_record_srv_t record = mdns_record_parse_srv(data, size, record_offset, record_length, srvBuffer.data(), 
                sizeof(char) * srvBuffer.size());

            // Store the parsed SRV record
            extractor->m_srvRecords.push_back({record.priority, record.weight, record.port, 
                std::string(record.name.str, record.name.length)});

            break;
        }
        
        // Any available records
        case MDNS_RECORDTYPE_ANY:
        {
            throw std::runtime_error("MDNS_RECORDTYPE_ANY is not supported.");
        }

    default:
        throw std::runtime_error("Unknown record type.");
    }

    // Return 0 to indicate that we have handled the record
    return 0;
}

std::string mDNSRecordExtractor::ExtractString_mDNS(const void* data, size_t size, size_t offset)
{
    // Allocate a buffer for the string of size 256 bytes since a 
    // full domain name can be up to 255 octets
    std::array<char, 256> nameStringBuffer;
    // Extract the string from the mDNS data
    // (this uses an internal function from the mdns library and may not be desirable)
    mdns_string_t recordName_mdns = mdns_string_extract(data, sizeof(nameStringBuffer), 
        &offset, nameStringBuffer.data(), sizeof(char) * nameStringBuffer.size());
    
    // Convert the parsed mdns string to a C++ string
    return std::string(recordName_mdns.str, recordName_mdns.length);
}

Address mDNSRecordExtractor::SockaddrToAddress(const sockaddr_in& sockaddr, size_t addressLength)
{
    std::array<char, NI_MAXHOST> host;
    std::array<char, NI_MAXSERV> service;

    // Attempt to convert the sockaddr_in structure to a human-readable address
    // using the host and service buffers
    if (getnameinfo(reinterpret_cast<const struct sockaddr*>(&sockaddr), 
                    static_cast<socklen_t>(addressLength), host.data(), NI_MAXHOST, 
                    service.data(), NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) != 0)
    {
        throw std::runtime_error("Failed to convert sockaddr to Address.");
    }

    // Create an Address object from the host and service buffers
    uint16_t port = static_cast<uint16_t>(std::stoul(service.data()));
    return Address(host.data(), port);
}

Address mDNSRecordExtractor::SockaddrToAddress(const sockaddr_in6& sockaddr, size_t addressLength)
{
    std::array<char, NI_MAXHOST> host;
    std::array<char, NI_MAXSERV> service;
    
    // Attempt to convert the sockaddr_in6 structure to a human-readable address
    // using the host and service buffers
    if (getnameinfo(reinterpret_cast<const struct sockaddr*>(&sockaddr),
                    static_cast<socklen_t>(addressLength), host.data(), NI_MAXHOST,
                    service.data(), NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) != 0)
    {
        // Consider more specific error handling or logging
        throw std::runtime_error("Failed to convert sockaddr_in6 to Address.");
    }

    // Create an Address object from the host and service buffers
    uint16_t port = static_cast<uint16_t>(std::stoul(service.data()));
    return Address(host.data(), port);
}
