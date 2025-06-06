#pragma once

// STL includes
#include <string>

namespace MoonlightOBS
{
    /**
     * @brief Static helper class for string manipulation and utilities.
     * 
     */
    class StringTools
    {
    public:
        /**
         * @brief Strips a string from within a given string.
         * 
         * @param from The string to strip from.
         * @param str The string to strip.
         * @return std::string The string without the stripped content.
         */
        inline static std::string StripFrom(std::string_view from, std::string_view str)
        {
            // Check if the input strings are empty
            if (from.empty() || str.empty())
            {
                return std::string(from);
            }
    
            // Find the string to strip
            size_t pos = from.find(str);
            if (pos == std::string_view::npos)
            {
                // String to strip not found, return original string
                return std::string(from);
            }
            
            // Strip the specified string from the original string
            std::string result(from);
            result.erase(pos, str.length());
            return result;
        }
        
        /**
         * @brief Strips a string from the end of another string.
         * 
         * @param from The string to strip from.
         * @param str The string to strip.
         * @return std::string The string without the stripped content.
         */
        inline static std::string RemoveSuffix(std::string_view str, std::string_view suffix)
        {
            // Check if the input strings are empty
            if (str.empty() || suffix.empty())
            {
                return std::string(str);
            }

            // Check if the string ends with the specified suffix
            if (str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0)
            {
                // Remove the suffix from the string
                return std::string(str.substr(0, str.size() - suffix.size()));
            }
            
            // If it doesn't end with the suffix, return the original string
            return std::string(str);
        }

        /**
         * @brief Counts the occurrences of a substring within a string.
         * 
         * @param str The string to search in.
         * @param subStr The substring to count occurrences of.
         * @return size_t The number of occurrences of the substring.
         */
        inline static size_t CountOccurrences(std::string_view str, std::string_view subStr)
        {
            // Check if the input strings are empty
            if (str.empty() || subStr.empty())
            {
                return 0;
            }
            
            size_t count    = 0;
            size_t pos      = 0;

            // Loop to find all occurrences of the substring
            while ((pos = str.find(subStr, pos)) != std::string_view::npos)
            {
                ++count;
                pos += subStr.length(); // Move past the last found substring
            }
            
            return count;
        }

        /**
         * @brief Deleted constructors and assignment operators to prevent instantiation.
         * 
         */
        StringTools() = delete;
        /**
         * @brief Deleted copy constructor to prevent copying.
         * 
         */
        StringTools(const StringTools&) = delete;
        /**
         * @brief Deleted move constructor to prevent moving.
         * 
         */
        StringTools(StringTools&&) = delete;
        /**
         * @brief Deleted assignment operator to prevent assignment.
         * 
         * @return StringTools& Other instance to assign from.
         */
        StringTools& operator=(const StringTools&) = delete;
        /**
         * @brief Deleted move assignment operator to prevent move assignment.
         * 
         * @return StringTools& Other instance to assign from.
         */
        StringTools& operator=(StringTools&&) = delete;
        /**
         * @brief Deleted destructor to prevent destruction.
         * 
         */
        ~StringTools() = delete;
    };
}
