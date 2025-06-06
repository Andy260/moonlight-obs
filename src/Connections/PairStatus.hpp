#pragma once

namespace MoonlightOBS
{
    /**
     * @brief Represents the status of the pairing between a GameStream host and a client.
     * 
     */
    enum class PairStatus
    {
        /**
         * @brief The host is not paired with the client.
         * 
         */
        Unpaired,
        /**
         * @brief The host is paired with the client.
         * 
         */
        Paired
    };
}
