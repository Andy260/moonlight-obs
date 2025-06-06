#pragma once

// STL includes
#include <string>
#include <string_view>

namespace MoonlightOBS
{
    /**
     * @brief Represents a version using the semantic versioning format.
     * 
     */
    class Version
    {
    public:
        /**
         * @brief Constructs a new Version object with the specified version numbers.
         * 
         * @param major The major version number.
         * @param minor The minor version number.
         * @param build The build number.
         */
        Version(int major, int minor, int patch);

        /**
         * @brief Constructs a new Version object with the specified version numbers.
         * 
         * @param major The major version number.
         * @param minor The minor version number.
         * @param build The build number.
         * @param revision The revision number.
         */
        Version(int major, int minor, int build, int revision);

        /**
         * @brief Creates a version object from a string representation.
         * 
         * @param versionString The string representation of the version in the format "major.minor.patch" 
         *                      or "major.minor[.build[.revision]]" formats.
         * @return Version The version object created from the string.
         */
        static Version FromString(std::string_view versionString);

        /**
         * @brief Gets a version object representing an unknown version.
         * 
         * @return Version The version object representing an unknown version.
         */
        inline static Version GetUnknownVersion()
        {
            // Returns a version object representing an unknown version
            return Version(-1, -1, -1, -1);
        }

        /**
         * @brief Gets the major version number.
         * 
         * @return int The major version number.
         */
        inline int GetMajor() const
        {
            return m_major;
        }

        /**
         * @brief Gets the minor version number.
         * 
         * @return int The minor version number.
         */
        inline int GetMinor() const
        {
            return m_minor;
        }

        /**
         * @brief Gets the build number.
         * 
         * @return int The build number.
         */
        inline int GetBuild() const
        {
            return m_build;
        }

        /**
         * @brief Gets the revision number.
         * 
         * @return int The revision number.
         */
        inline int GetRevision() const
        {
            return m_revision;
        }

        /**
         * @brief Converts the version to a string representation.
         * 
         * @return std::string The string representation of the version.
         */
        std::string ToString() const;

        /**
         * @brief Compares this version with another version for equality.
         * 
         * @param other The other version to compare with.
         * @return true This version is equal to the other version.
         * @return false This version is not equal to the other version.
         */
        inline bool operator==(const Version& other) const
        {
            return (m_major == other.m_major && m_minor == other.m_minor &&
                    m_build == other.m_build && m_revision == other.m_revision);
        }
        /**
         * @brief Compares this version with another version for inequality.
         * 
         * @param other The other version to compare with.
         * @return true This version is not equal to the other version.
         * @return false This version is equal to the other version.
         */
        inline bool operator!=(const Version& other) const
        {
            return !(*this == other);
        }
        /**
         * @brief Compares this version with another version for less than.
         * 
         * @param other The other version to compare with.
         * @return true This version is less than the other version.
         * @return false This version is not less than the other version.
         */
        bool operator<(const Version& other) const;
        /**
         * @brief Compares this version with another version for less than or equal to.
         * 
         * @param other The other version to compare with.
         * @return true This version is less than or equal to the other version.
         * @return false This version is greater than the other version.
         */
        inline bool operator<=(const Version& other) const
        {
            // If this version is less than the other version, or they are equal
            return (*this < other) || (*this == other);
        }
        /**
         * @brief Compares this version with another version for greater than.
         * 
         * @param other The other version to compare with.
         * @return true This version is greater than the other version.
         * @return false This version is not greater than the other version.
         */
        inline bool operator>(const Version& other) const
        {
            return !(*this <= other);
        }
        /**
         * @brief Compares this version with another version for greater than or equal to.
         * 
         * @param other The other version to compare with.
         * @return true This version is greater than or equal to the other version.
         * @return false This version is less than the other version.
         */
        inline bool operator>=(const Version& other) const
        {
            return !(*this < other);
        }
        
    private:
        // Major version number
        int m_major;
        // Minor version number
        int m_minor;
        // Build version number
        int m_build;
        // Revision version number
        int m_revision;

        static std::string GetComponentName(int component);
    };
}
