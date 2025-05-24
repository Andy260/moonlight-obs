#pragma once

// STL includes
#include <cstdint>
#include <string>

namespace MoonlightOBS
{
    /**
     * @brief Represents a service record (SRV) within the mDNS protocol.
     * 
     */
    class SRVRecord
    {
    public:
        /**
         * @brief Constructor for the SRVRecord class.
         * 
         * @param priority The priority of the service.
         * @param weight The weight of the service.
         * @param port The port number of the service.
         * @param target The target hostname of the service.
         */
        SRVRecord(uint16_t priority, uint16_t weight, uint16_t port, const std::string_view& target)
            : m_priority(priority), m_weight(weight), m_port(port), m_target(target) {}

        /**
         * @brief Get the Priority of the service.
         * 
         * @return uint16_t The priority of the service.
         */
        inline uint16_t GetPriority() const 
        { 
            return m_priority; 
        }

        /**
         * @brief Get the Weight of the service.
         * 
         * @return uint16_t The weight of the service.
         */
        inline uint16_t GetWeight() const 
        { 
            return m_weight; 
        }

        /**
         * @brief Get the Port number of the service.
         * 
         * @return uint16_t The port number of the service.
         */
        inline uint16_t GetPort() const 
        { 
            return m_port; 
        }

        /**
         * @brief Get the Target hostname of the service.
         * 
         * @return const std::string& The target hostname of the service.
         */
        inline const std::string& GetTarget() const 
        { 
            return m_target; 
        }

    private:
        // Priority of the service
        uint16_t m_priority;
        // Weight of the service
        uint16_t m_weight;
        // Port number of the service
        uint16_t m_port;
        // Target hostname of the service
        std::string m_target;
    };
}
