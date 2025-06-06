#pragma once

// STL includes
#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <vector>

namespace MoonlightOBS
{
    // Forward declarations
    class Address;
    class GameStreamHost;
    class SRVRecord;

    /**
     * @brief Static helper class to find GameStream hosts on the local network.
     * 
     */
    class LANSearcher
    {
    public:
        /**
         * @brief Starts searching for GameStream hosts on the local network.
         * 
         * @param callback The callback function to be called when a host 
         *                 is found, with the host as the parameter to the 
         *                 callback function.
         * 
         * @exception std::logic_error If the search is already running.
         *                             -or-
         *                             If the callback is null.
         * 
         * @exception std::runtime_error If the search fails to start.      
         */
        static void Start(std::function <void(const GameStreamHost&)> callback);

        /**
         * @brief Stops searching for GameStream hosts on the local network.
         * 
         * @exception std::runtime_error If the search is not running.
         */
        static void Stop();

        /**
         * @brief Is the search for GameStream hosts currently running?
         * 
         * @return true if the search is running.
         *         -or-
         *         false if the search is not running.
         */
        inline static bool IsSearching()
        {
            return m_searching.load(std::memory_order_acquire);
        }

    private:
        // Private constructor and destructor to prevent instantiation
        LANSearcher()                               = delete;
        ~LANSearcher()                              = delete;
        LANSearcher(const LANSearcher&)             = delete;
        LANSearcher& operator=(const LANSearcher&)  = delete;
        LANSearcher(LANSearcher&&)                  = delete;
        LANSearcher& operator=(LANSearcher&&)       = delete;

        // The thread used for searching for GameStream hosts
        static std::thread m_searchThread;
        // Flag to indicate if the search is running
        static std::atomic_bool m_searching;

        // Function invoked by the search thread
        static void SearchThread(std::function<void(const GameStreamHost&)> callback, 
            int ipv4Socket, int ipv6Socket);
        
        // Function used discover the instance names of the available GameStream hosts
        static std::vector<std::string> DiscoverInstanceNames(int socket);
        // Function used to discover the mDNS hostname and port of a discovered GameStream host
        static SRVRecord ResolvemDNSHostname(const std::string_view& serviceName, int socket);
        // Function used to resolve the hostname of the GameStream host 
        // using the /serverinfo endpoint of the host
        static std::string ResolveHostname(const Address& address);
        // Function used to resolve the IP address of a discovered GameStream host
        static Address ResolveIPAddress(const GameStreamHost& host, int socket, bool useIPv6);

        // Function used to log the host discovery
        static void LogHost(int level, const std::string_view& message, GameStreamHost host, 
            const std::string_view& serviceName);
    };
} // namespace MoonlightOBS
