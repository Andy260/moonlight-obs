#pragma once

// STL includes
#include <string>
#include <vector>
#include <utility>

// mdns includes
#include <mdns.h>

// Project includes
#include "../Connections/Address.hpp"
#include "SRVRecord.hpp"

// Forward declarations
struct sockaddr_in;
struct sockaddr_in6;

namespace MoonlightOBS
{
    // Forward declarations
    class Address;

    /**
     * @brief Helper class to extract mDNS records from a packet.
     * 
     */
    class mDNSRecordExtractor
    {
    public:
        /**
         * @brief Destructor for the mDNSRecordExtractor class.
         */
        ~mDNSRecordExtractor() = default;

        /**
         * @brief Extracts the mDNS records from the received response 
         *        to the unicast request.
         * 
         * @param socket The mDNS socket ID to send to mDNS.
         * @param queryID_filter The ID of the query to filter the response, 
         *                       or 0 to receive all responses.
         * @param service_filter The name of the service to filter the response.
         *                       (By default, it will receive all services)
         * @param entryType_filterMask Bitmask filter which entry types to handle.
         *                             (By default, it handle all types of entries.) 
         * 
         * @return mDNSRecordExtractor object containing the extracted records.
         * 
         * @exception std::invalid_argument If the socket is invalid.
         */
        static mDNSRecordExtractor Extract(int socket, int queryID_filter = 0, std::string service_filter = "",
            int entryType_filterMask = MDNS_ENTRYTYPE_QUESTION | MDNS_ENTRYTYPE_ANSWER | MDNS_ENTRYTYPE_AUTHORITY | MDNS_ENTRYTYPE_ADDITIONAL
        );
        
        /**
         * @brief Gets the received PTR records.
         *        (Domain Name pointer)
         * 
         * @return const std::vector<std::string>& Reference to the vector of received PTR records.
         */
        const std::vector<std::string>& GetPTRRecords() const
        {
            return m_ptrRecords;
        }

        /**
         * @brief Gets the received A records.
         *        (IPv4 Address)
         * 
         * @return const std::vector<Address>& Reference to the vector of received A records.
         */
        const std::vector<Address>& GetARecords() const
        {
            return m_ipv4Records;
        }

        /**
         * @brief Gets the received TXT records.
         *        (Arbitrary text string)
         * 
         * @return const std::vector<std::pair<std::string, std::string>>& Reference to the vector of received TXT records.
         */
        const std::vector<std::pair<std::string, std::string>>& GetTXTRecords() const
        {
            return m_txtRecords;
        }

        /**
         * @brief Gets the received AAAA records.
         *       (IPv6 Address)
         * 
         * @return const std::vector<Address>& Reference to the vector of received AAAA records.
         */
        const std::vector<Address>& GetAAAARecords() const
        {
            return m_ipv6Records;
        }

        /**
         * @brief Gets the received SRV records.
         *        (Server Selection)
         * 
         * @return const std::vector<SRVRecord>& Reference to the vector of received SRV records.
         */
        const std::vector<SRVRecord>& GetSRVRecords() const
        {
            return m_srvRecords;
        }

    private:
        // Private constructor to prevent instantiation
        mDNSRecordExtractor(std::string service_filter, int entryType_filterMask);

        // Number of responses handled
        size_t m_responsesHandled;
        // Bitmask filter which entry types to handle
        int m_entryType_filterMask;
        // The name of the service to filter the response
        std::string m_service_filter;

        // Received PTR records (Domain Name pointer)
        std::vector<std::string> m_ptrRecords;
        // Received A records (IPv4 Address)
        std::vector<Address> m_ipv4Records;
        // Received TXT records (Arbitrary text string)
        std::vector<std::pair<std::string, std::string>> m_txtRecords;
        // Received AAAA records (IPv6 Address)
        std::vector<Address> m_ipv6Records;
        // Received SRV records (Server Selection)
        std::vector<SRVRecord> m_srvRecords;

        /**
         * @brief Extracts mDNS records from a packet.
         * 
         * @param sock The socket to use for the mDNS query.
         * @param from The address of the sender.
         * @param addrlen The length of the address.
         * @param entry The type of mDNS entry.
         * @param query_id The ID of the query.
         * @param rtype The type of record.
         * @param rclass The class of record.
         * @param ttl The time to live of the record.
         * @param data The data of the record.
         * @param size The size of the data.
         * @param name_offset The offset of the name in the data.
         * @param name_length The length of the name in the data.
         * @param record_offset The offset of the record in the data.
         * @param record_length The length of the record in the data.
         * @param user_data User data to pass to the callback function.
         * 
         * @return 0 on indicate the record was handled, -1 on error.
         */
        static int OnCallback(int sock, const struct sockaddr* from, size_t addrlen,
                               mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype,
                               uint16_t rclass, uint32_t ttl, const void* data, size_t size,
                               size_t name_offset, size_t name_length, size_t record_offset,
                               size_t record_length, void* user_data);

        // Converts buffer to a string
        static std::string ExtractString_mDNS(const void* data, size_t size, size_t offset);
        // Converts sockaddr_in to Address
        static Address SockaddrToAddress(const sockaddr_in& sockaddr, size_t addressLength);
        // Converts sockaddr_in6 to Address
        static Address SockaddrToAddress(const sockaddr_in6& sockaddr, size_t addressLength);
    };
} // namespace MoonlightOBS
