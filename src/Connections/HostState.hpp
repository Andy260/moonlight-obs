#pragma once

namespace MoonlightOBS
{
    /**
     * @brief The current streaming state of the GameStream host.
     * 
     */
    enum class HostState
    {
        /**
         * @brief The GameStream host is not currently streaming a game.
         * @note Equivalent to "SUNSHINE_STATE_FREE" in the Sunshine protocol.
         */
        SERVER_FREE,
        /**
         * @brief The GameStream host is currently streaming a game.
         * @note Equivalent to "SUNSHINE_STATE_BUSY" in the Sunshine protocol.
         */
        SERVER_BUSY,
    };
}
