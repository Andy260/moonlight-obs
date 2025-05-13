#pragma once

// STL includes
#include <atomic>
#include <functional>
#include <string>
#include <thread>

namespace MoonlightOBS
{
    // Forward declarations
    class Host;

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
         *                 is found, with the host as a parameter.
         * 
         * @exception std::runtime_error If the search is already running.
         *                               -or-
         *                               If the callback is null.      
         */
        static void Start(std::function <void(const Host&)> callback);

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
            return m_searching.load();
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
    };
} // namespace MoonlightOBS
