#include "LANSearcher.hpp"

// STL includes
#include <atomic>
#include <functional>
#include <stdexcept>
#include <string>
#include <thread>

// OBS Studio includes
#include <util/base.h>

// Project includes
#include "../plugin-support.h"
#include "Host.hpp"

using namespace MoonlightOBS;

std::thread LANSearcher::m_searchThread;
std::atomic_bool LANSearcher::m_searching{false};

void LANSearcher::Start(std::function<void(const Host&)> callback)
{
    // Ensure callback is not null
    if (callback == nullptr)
    {
        throw std::runtime_error("Callback function cannot be null.");
    }
    // Check if search is already running
    else if (m_searching.load(std::memory_order_acquire))
    {
        throw std::runtime_error("Search is already running.");
    }

    // Start the search thread
    m_searchThread = std::thread([callback]()
    {
        // Set the searching flag to true
        m_searching.store(true, std::memory_order_release);

        // Simulate finding the local host
        std::this_thread::sleep_for(std::chrono::seconds(1));
        callback(Host(Address("127.0.0.1", 47989), "Test Host"));

        // This thread is intended to run indefinitely until stopped
        while (m_searching.load(std::memory_order_acquire))
        {
            // TODO: Implement the actual search logic here
            obs_log(LOG_INFO, "Searching for GameStream hosts...");

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    // Detach the thread to allow it to run independently
    m_searchThread.detach();
}

void LANSearcher::Stop()
{
    // Check if search is running
    if (!m_searching.load(std::memory_order_acquire))
    {
        throw std::runtime_error("Search is not running.");
    }

    // Signal the search thread to stop
    m_searching.store(false, std::memory_order_release);

    // Stop the search thread
    if (m_searchThread.joinable())
    {
        m_searchThread.join();
    }
}
