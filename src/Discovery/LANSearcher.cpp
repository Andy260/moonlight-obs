#include "LANSearcher.hpp"

// STL includes
#include <array>
#include <atomic>
#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

// OBS Studio includes
#include <util/base.h>

// mdns includes
#include <mdns.h>

// Project includes
#include "../plugin-support.h"
#include "../Connections/GameStreamHost.hpp"
#include "mDNSRecordExtractor.hpp"

using namespace MoonlightOBS;

std::thread LANSearcher::m_searchThread;
std::atomic_bool LANSearcher::m_searching{false};

void LANSearcher::Start(std::function<void(const GameStreamHost&)> callback)
{
    // Ensure callback is not null
    if (callback == nullptr)
    {
        throw std::logic_error("Callback function cannot be null.");
    }
    // Check if search is already running
    else if (m_searching.load(std::memory_order_acquire))
    {
        throw std::logic_error("Search is already running.");
    }

    // Log the start of the search
    obs_log(LOG_INFO, "Starting search for GameStream hosts...");

    // Open the ipv4 socket for mDNS
    int ipv4Socket = mdns_socket_open_ipv4(nullptr);
    // Open the ipv6 socket for mDNS
    int ipv6Socket = mdns_socket_open_ipv6(nullptr);

    // Ensure at least one socket was created successfully
    if (ipv4Socket < 0 && ipv6Socket < 0)
    {
        obs_log(LOG_ERROR, "Aborting search for GameStream hosts due to socket creation failure.");
        m_searching.store(false, std::memory_order_release);
        
        throw std::runtime_error("Failed to create mDNS sockets.");
    }
    else if (ipv4Socket < 0 && ipv6Socket >= 0)
    {
        // Warn the user that only IPv6 will be used
        obs_log(LOG_WARNING, "Unable to create IPv4 mDNS socket, only searching with IPv6.");
    }
    else if (ipv6Socket < 0 && ipv4Socket >= 0)
    {
        // Warn the user that only IPv4 will be used
        obs_log(LOG_WARNING, "Unable to create IPv6 mDNS socket, only searching with IPv4.");
    }

    // Set the searching flag to true
    m_searching.store(true, std::memory_order_release);
    // Start the search thread
    m_searchThread = std::thread(LANSearcher::SearchThread, callback, ipv4Socket, ipv6Socket);
    
    // Detach the thread to allow it to run independently
    m_searchThread.detach();
}

void LANSearcher::SearchThread(std::function<void(const GameStreamHost&)> callback, int ipv4Socket, int ipv6Socket)
{
    // Hosts that have been found and have been 
    // notified to the callback function
    // (Service name / GameStreamHost)
    std::map<std::string, GameStreamHost> foundHosts;

    // Loop until the search is stopped
    do
    {
        // In order to discover the GameStream hosts we need to perform the following steps:
        // 1. Send an mDNS query to discover the instance names of the available GameStream hosts
        //    by sending a PTR query to the _nvstream._tcp.local. service
        // 2. Resolve the hostnames of the found hosts by sending by sending a SRV query to the
        //    _nvstream._tcp.local. services found in the previous step
        // 3. Resolve the IP addresses of the found hosts by sending by sending an A or AAAA query
        //    to the hostnames found in the previous step
        //    (IPv4 or IPv6 depending on the socket used)
        // 4. Notify the callback function with the found hosts

        // Allocate a map to store the discovered GameStream hosts
        // (Service name / GameStreamHost)
        std::map<std::string, GameStreamHost> discoveredHosts;

        // Send the mDNS query to discover the instance names of new GameStream hosts
        // using the IPv4 socket
        if (ipv4Socket >= 0)
        {
            try
            {
                // Discover the instance names of GameStream hosts
                std::vector<std::string> discoveredServices = DiscoverInstanceNames(ipv4Socket);
                
                // Add the discovered hosts to the vector to be processed
                // if the host is not already in the found hosts map
                for (const auto& service : discoveredServices)
                {
                    // Check if the host is already in the found hosts map
                    if (foundHosts.find(service) != foundHosts.end())
                    {
                        // Host already exists, skip it
                        continue;
                    }

                    // Create a new GameStreamHost object from the service name
                    discoveredHosts.try_emplace(service, GameStreamHost::GetEmpty());
                }
            }
            catch(const std::runtime_error& exception)
            {
                UNUSED_PARAMETER(exception);

                // Query failed, log the error
                obs_log(LOG_ERROR, "Failed to query for IPv4 hosts on socket: %i", ipv4Socket);
            }
        }
        // Send the mDNS query to discover the instance names of new GameStream hosts
        // using the IPv6 socket
        if (ipv6Socket >= 0)
        {
            try
            {
                // Discover the instance names of GameStream hosts
                std::vector<std::string> discoveredServices = DiscoverInstanceNames(ipv6Socket);
                
                // Add the discovered hosts to the vector to be processed
                // if the host is not already in the found hosts map
                for (const auto& service : discoveredServices)
                {
                    // Check if the host is already in the found hosts map
                    if (foundHosts.find(service) != foundHosts.end() || 
                        discoveredHosts.find(service) != discoveredHosts.end()) 
                    {
                        // Host is already discovered and resolved or 
                        // discovered by the ipv4 query, skip it
                        continue;
                    }

                    // Create a new GameStreamHost object from the service name
                    discoveredHosts.try_emplace(service, GameStreamHost::GetEmpty());
                }
            }
            catch(const std::runtime_error& exception)
            {
                UNUSED_PARAMETER(exception);

                // Query failed, log the error
                obs_log(LOG_ERROR, "Failed to query for IPv6 hosts on socket: %i", ipv6Socket);
            }
        }
        
        // Resolve the discovered hosts
        for (auto& [serviceName, host] : discoveredHosts)
        {
            // Resolved hostname and port from the IPv4 socket
            std::string ipv4Hostname    = "";
            uint16_t ipv4Port           = -1;
            // Resolved hostname and port from the IPv6 socket
            std::string ipv6Hostname    = "";
            uint16_t ipv6Port           = -1;

            // Attempt to resolve the hostname of the host
            // using the IPv4 socket
            if (ipv4Socket >= 0)
            {
                try
                {
                    // Resolve the hostname of the host using the IPv4 socket
                    SRVRecord srvRecord = ResolveHostname(serviceName, ipv4Socket);
                    
                    // Strip the .local. suffix from the hostname
                    std::string hostname = srvRecord.GetTarget();
                    const std::string suffix = ".local.";
                    if (hostname.length() >= suffix.length() && 
                        hostname.compare(hostname.length() - suffix.length(), suffix.length(), suffix) == 0)
                    {
                        // Remove the .local. suffix from the hostname
                        hostname.erase(hostname.length() - suffix.length());
                    }

                    // Save the resolved hostname
                    ipv4Hostname = hostname;
                    // Save the resolved port
                    ipv4Port = srvRecord.GetPort();
                }
                catch (const std::runtime_error& exception)
                {
                    UNUSED_PARAMETER(exception);

                    // Query failed, log the error
                    obs_log(LOG_WARNING, "Failed to resolve hostname for service '%s' on IPv4 socket: %i", 
                        serviceName.c_str(), ipv4Socket);
                }
            }
            // Attempt to resolve the hostname of the host
            // using the IPv6 socket (if not resolved by the IPv4 socket)
            if (ipv6Socket >= 0 && ipv4Hostname.empty())
            {
                try
                {
                    // Resolve the hostname of the host using the IPv4 socket
                    SRVRecord srvRecord = ResolveHostname(serviceName, ipv6Socket);

                    // Strip the .local. suffix from the hostname
                    std::string hostname = srvRecord.GetTarget();
                    const std::string suffix = ".local.";
                    if (hostname.length() >= suffix.length() && 
                        hostname.compare(hostname.length() - suffix.length(), suffix.length(), suffix) == 0)
                    {
                        // Remove the .local. suffix from the hostname
                        hostname.erase(hostname.length() - suffix.length());
                    }

                    // Save the resolved hostname
                    ipv6Hostname = hostname;
                    // Save the resolved port
                    ipv6Port = srvRecord.GetPort();
                }
                catch (const std::runtime_error& exception)
                {
                    UNUSED_PARAMETER(exception);

                    // Query failed, log the error
                    obs_log(LOG_WARNING, "Failed to resolve hostname for service '%s' on IPv6 socket: %i", 
                        serviceName.c_str(), ipv6Socket);
                }
            }

            // Calculate the expected hostname based on the service name
            std::string expectedHostname = serviceName;
            // Strip the _nvstream._tcp.local. postfix from the service name
            expectedHostname = expectedHostname.erase(serviceName.find("._nvstream._tcp.local."), 
                std::string("._nvstream._tcp.local.").length());

            if (ipv4Hostname == ipv6Hostname)
            {
                // Both hostnames are the same
                if (ipv4Hostname == expectedHostname)
                {
                    // This is the best case scenario, all hostnames match
                    host.SetHostname(ipv4Hostname);
                    // Set the IPv4 address
                    host.SetIPv4Address(Address("", ipv4Port));
                    // Set the IPv6 address
                    host.SetIPv6Address(Address("", ipv6Port));
                }
                else
                {
                    // Both resolved IPv4 and IPv6 hostnames do not 
                    // match the expected hostname, log the error
                    std::stringstream errorStream;

                    // Log format: Resolved hostnames for service '<serviceName>' do not match the expected hostname '<expectedHostname>'.
                    // This is likely due to a misconfiguration of the GameStream host. (IPv4: <ipv4Hostname>, IPv6: <ipv6Hostname>)
                    errorStream << "Resolved hostnames for service '";
                    errorStream << serviceName << "' do not match the expected hostname '";
                    errorStream << expectedHostname << "'. ";
                    errorStream << "This is likely due to a misconfiguration of the GameStream host. ";
                    errorStream << "(IPv4: " << ipv4Hostname << ", IPv6: " << ipv6Hostname << ")";
                    // Send the error log to OBS
                    obs_log(LOG_ERROR, "%s", errorStream.str().c_str());

                    // Skip this host
                    continue;
                }
            }
            else
            {
                // Getting to this point means that the hostnames resolved are different
                // so we must choose one of them to use as the hostname (assuming at least one is valid)

                if (ipv4Hostname == expectedHostname)
                {
                    // Use the IPv4 hostname as it matches the expected hostname
                    host.SetHostname(ipv4Hostname);
                    // Set the IPv4 port
                    host.SetIPv4Address(Address("", ipv4Port));
                }
                else if (ipv6Hostname == expectedHostname)
                {
                    // Use the IPv6 hostname as it matches the expected hostname
                    host.SetHostname(ipv6Hostname);
                    // Set the IPv6 port
                    host.SetIPv6Address(Address("", ipv6Port));
                }
                else
                {
                    // Neither hostname matches the expected hostname, log the error
                    std::stringstream errorStream;

                    // Log format: Resolved hostnames for service '<serviceName>' do not match the expected hostname '<expectedHostname>'.
                    // This is likely due to a misconfiguration of the GameStream host. (IPv4: <ipv4Hostname>, IPv6: <ipv6Hostname>)
                    errorStream << "Resolved hostnames for service '";
                    errorStream << serviceName << "' do not match the expected hostname '";
                    errorStream << expectedHostname << "'. ";
                    errorStream << "This is likely due to a misconfiguration of the GameStream host. ";
                    errorStream << "(IPv4: " << ipv4Hostname << ", IPv6: " << ipv6Hostname << ")";
                    // Send the error log to OBS
                    obs_log(LOG_ERROR, "%s", errorStream.str().c_str());

                    // Skip this host
                    continue;
                }
            }

            // Check if the hostname was resolved successfully
            if (host.GetHostname().empty())
            {
                // Hostname could not be resolved, log the error
                obs_log(LOG_ERROR, "Failed to resolve hostname for service '%s'", serviceName.c_str());
                // Skip this host
                continue;
            }

            // Resolve the IPv4 addresses of the host
            if (ipv4Socket >= 0)
            {
                try
                {
                    // Resolve the IPv4 address of the host
                    Address ipv4Address = ResolveIPAddress(host, ipv4Socket, false);
                    
                    // Update the address with the previously resolved port
                    ipv4Address.SetPortNumber(host.GetIPv4Address().GetPortNumber());
                    // Set the IPv4 address of the host
                    host.SetIPv4Address(ipv4Address);
                }
                catch (const std::runtime_error& exception)
                {
                    UNUSED_PARAMETER(exception);

                    // Log the error if the IPv4 address could not be resolved
                    obs_log(LOG_WARNING, "Failed to resolve IPv4 address for host: %s (Service Name: %s)", 
                        host.GetHostname().c_str(), serviceName.c_str());
                }
            }
            // Resolve the IPv6 addresses of the host
            if (ipv6Socket >= 0)
            {
                try
                {
                    // Resolve the IPv6 address of the host
                    Address ipv6Address = ResolveIPAddress(host, ipv6Socket, true);
                    
                    // Update the address with the previously resolved port
                    ipv6Address.SetPortNumber(host.GetIPv6Address().GetPortNumber());
                    // Set the IPv4 address of the host
                    host.SetIPv6Address(ipv6Address);
                }
                catch (const std::runtime_error& exception)
                {
                    UNUSED_PARAMETER(exception);

                    // Log the error if the IPv6 address could not be resolved
                    obs_log(LOG_WARNING, "Failed to resolve IPv6 address for host: %s (Service Name: %s)", 
                        host.GetHostname().c_str(), serviceName.c_str());
                }
            }

            // Set the unknown addresses to empty if they were not resolved
            if (host.GetIPv4Address().GetAddress().empty())
            {
                host.SetIPv4Address(Address::GetEmpty());
            }
            if (host.GetIPv6Address().GetAddress().empty())
            {
                host.SetIPv6Address(Address::GetEmpty());
            }
            
            // Check if the host was resolved successfully
            if (!host.IsValid())
            {
                LogHost(LOG_ERROR, "Failed to resolve host", host, serviceName);

                // Skip this host
                continue;
            }

            // Add the host to the found hosts map
            foundHosts.try_emplace(serviceName, host);

            // Log the resolved host
            std::stringstream logStream;

            // Log the resolved host with its service name and addresses
            LogHost(LOG_INFO, "Found GameStream host", host, serviceName);
            
            // Alert callback function with the found host
            callback(host);
        }
    } while (m_searching.load(std::memory_order_acquire));

    // Close the mDNS sockets
    if (ipv4Socket >= 0)
    {
        mdns_socket_close(ipv4Socket);
    }
    if (ipv6Socket >= 0)
    {
        mdns_socket_close(ipv6Socket);
    }

    // Log the end of the search
    obs_log(LOG_INFO, "Stopped searching for GameStream hosts.");
}

std::vector<std::string> LANSearcher::DiscoverInstanceNames(int socket)
{
    // Ensure the socket is valid
    if (socket < 0)
    {
        throw std::invalid_argument("Socket is invalid.");
    }

    // Name of the mDNS service to search for
    std::string serviceName = "_nvstream._tcp.local.";

    // Allocate data for the mDNS query
    int sendStatus  = -1;
    int queryID     = 0;
    std::array<char, 2048> packetBuffer;

    // Send the mDNS query to discover GameStream hosts
    sendStatus = mdns_query_send(socket, MDNS_RECORDTYPE_PTR, serviceName.c_str(), 
                serviceName.length(), packetBuffer.data(), sizeof(char) * packetBuffer.size(), 0);

    // Check the status of the query
    if (sendStatus < 0)
    {
        // Query failed
        throw std::runtime_error("Failed to send PTR query");
    }
    else
    {
        // Query sent successfully, store the query ID
        queryID = sendStatus;
    }

    // Add a small delay to allow the mDNS response to be received
    // (This is necessary because mDNS responses are sent asynchronously)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Extract the mDNS records from the response
    mDNSRecordExtractor ipv4Records = mDNSRecordExtractor::Extract(socket, queryID, serviceName, MDNS_ENTRYTYPE_ANSWER);

    // Return the discovered services
    return ipv4Records.GetPTRRecords();
}

SRVRecord LANSearcher::ResolveHostname(const std::string_view& serviceName, int socket)
{
    // Ensure the socket is valid
    if (socket < 0)
    {
        throw std::invalid_argument("Socket is invalid.");
    }

    // Allocate data for the mDNS query
    int sendStatus  = -1;
    int queryID     = 0;
    std::array<char, 512> packetBuffer;

    // Send the SRV query to resolve the hostname of the service
    sendStatus = mdns_query_send(socket, MDNS_RECORDTYPE_SRV, serviceName.data(), 
                serviceName.length(), packetBuffer.data(), sizeof(char) * packetBuffer.size(), 0);

    // Check the status of the query
    if (sendStatus < 0)
    {
        // Query failed
        throw std::runtime_error("Failed to send SRV query");
    }
    else
    {
        // Query sent successfully, store the query ID
        queryID = sendStatus;
    }

    // Add a small delay to allow the mDNS response to be received
    // (This is necessary because mDNS responses are sent asynchronously)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Extract the mDNS records from the response to the SRV query
    mDNSRecordExtractor records = mDNSRecordExtractor::Extract(socket, queryID, "", MDNS_ENTRYTYPE_ANSWER);
    const std::vector<MoonlightOBS::SRVRecord> srvRecords = records.GetSRVRecords();

    // Check if any SRV records were found
    if (srvRecords.empty())
    {
        throw std::runtime_error("No SRV records received");
    }

    // Return the first SRV record
    return srvRecords.front();
}

Address LANSearcher::ResolveIPAddress(const GameStreamHost& host, int socket, bool useIPv6)
{
    // Ensure the socket is valid
    if (socket < 0)
    {
        throw std::invalid_argument("Socket is invalid.");
    }

    // Allocate data for the mDNS query
    std::string hostname = host.GetHostname() + ".local."; // Suffix is stripped and needs to be added back
    int sendStatus  = -1;
    int queryID     = 0;
    std::array<char, 512> packetBuffer;

    // Send the mDNS query to resolve the host's IP address
    sendStatus = mdns_query_send(socket, !useIPv6 ? MDNS_RECORDTYPE_A : MDNS_RECORDTYPE_AAAA, 
        hostname.c_str(), hostname.length(), packetBuffer.data(), 
        sizeof(char) * packetBuffer.size(), 0);
    
    // Check the status of the query
    if (sendStatus < 0)
    {
        // Query failed, log the error
        if (!useIPv6)
        {
            // IPv4
            throw std::runtime_error("Failed to send A query");
        }
        else
        {
            // IPv6
            throw std::runtime_error("Failed to send AAAA query");
        }
    }
    else
    {
        // Query sent successfully, store the query ID
        queryID = sendStatus;
    }

    // Add a small delay to allow the mDNS response to be received
    // (This is necessary because mDNS responses are sent asynchronously)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Extract the mDNS records from the response to the ipv4 query
    mDNSRecordExtractor records = mDNSRecordExtractor::Extract(socket, queryID, "", MDNS_ENTRYTYPE_ANSWER);

    const std::vector<Address>& receivedRecords = !useIPv6 ? records.GetARecords() : records.GetAAAARecords();
    // Check if any records were found
    if (receivedRecords.empty())
    {
        // No records found, log the error
        if (!useIPv6)
        {
            // IPv4
            throw std::runtime_error("No A records received");
        }
        else
        {
            // IPv6
            throw std::runtime_error("No AAAA records received");
        }
    }
    
    // Return the first record
    return receivedRecords.front();
}

void LANSearcher::LogHost(int level, const std::string_view& message, GameStreamHost host, 
    const std::string_view& serviceName)
{
    // Logging format:
    //
    // <message>: <hostname> (Service Name: <serviceName>, IPv4: <ipv4Address>, IPv6: <ipv6Address>)

    // Create a string stream to begin building the log message
    std::stringstream logStream;
    logStream << message << ": " << host.GetHostname() 
        << " (Service: " << serviceName;

    // Add the IPv4 address (if any)
    if (host.GetIPv4Address().IsValid())
    {
        logStream << ", IPv4: " << host.GetIPv4Address().GetString();
    }
    // Add the IPv6 address (if any)
    if (host.GetIPv6Address().IsValid())
    {
        logStream << ", IPv6: " << host.GetIPv6Address().GetString();
    }
    // Close the log message
    logStream << ")";
            
    // Send the log message to OBS
    obs_log(level, logStream.str().c_str());
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

    // Wait for the search thread to finish
    if (m_searchThread.joinable())
    {
        m_searchThread.join();
    }
}
