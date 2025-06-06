#include "Version.hpp"

// STL includes
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// OBS Studio includes
#include <util/base.h>
#include <util/c99defs.h>

// Project includes
#include "../Utilities/StringTools.hpp"
#include "../plugin-support.h"

using namespace MoonlightOBS;

Version::Version(int major, int minor, int build)
    : m_major(major), m_minor(minor), m_build(build), m_revision(-1)
{
    // Validate the version numbers
    if (major < 0 || minor < 0 || build < 0)
    {
        throw std::invalid_argument("Version numbers must be non-negative.");
    }
}

Version::Version(int major, int minor, int build, int revision)
    : m_major(major), m_minor(minor), m_build(build), m_revision(revision)
{
    // Validate the version numbers
    if (major == -1 && minor == -1 && build == -1 && revision == -1)
    {
        // Special case for unknown version
        return;
    }
    else if (major > 0 && minor > 0 && build > 0 && revision == -1)
    {
        // Special case for Sunshine hosts as version numbers with 
        // the revision number being -1, is used to indicate that the host is Sunshine
        return;
    }
    else if (major < 0 || minor < 0 || build < 0 || revision < 0)
    {
        throw std::invalid_argument("Version numbers must be non-negative.");
    }
}

Version Version::FromString(std::string_view versionString)
{
    // Check if the version string is empty
    if (versionString.empty())
    {
        throw std::invalid_argument("Version string cannot be empty.");
    }
    else if (versionString == "unknown"     || 
             versionString == "-1.-1.-1"    || 
             versionString == "-1.-1.-1.-1")
    {
        // Return an unknown version
        return GetUnknownVersion();
    }
    // Ensure the version string is in the correct format
    size_t dotCount = StringTools::CountOccurrences(versionString, ".");
    if (dotCount < 3 || dotCount > 4)
    {
        throw std::invalid_argument("Version string must be in the format 'major.minor.patch' or 'major.minor[.build[.revision]]'.");
    }

    std::vector<int> versionComponents(dotCount + 1, -1);
    size_t startOffset  = 0;
    size_t endOffset    = versionString.find('.');

    for (size_t i = 0; i < versionComponents.size(); ++i)
    {
        // Check if the endOffset is valid
        if (endOffset == std::string_view::npos)
        {
            endOffset = versionString.length();
        }

        // Extract the component from the version string
        std::string_view component = versionString.substr(startOffset, endOffset - startOffset);

        // Convert the component to an integer
        try
        {
            versionComponents[i] = std::stoi(std::string(component));
        }
        catch (const std::invalid_argument& exception)
        {
            UNUSED_PARAMETER(exception);
            throw std::invalid_argument("Invalid version component: " + std::string(component));
        }
        catch (const std::out_of_range& exception)
        {
            UNUSED_PARAMETER(exception);
            throw std::invalid_argument("Version component out of range: " + std::string(component));
        }

        // Advance the offset variables to find the next component
        startOffset = endOffset + 1;
        endOffset   = versionString.find('.', startOffset);
    }

    return Version(versionComponents[0], versionComponents[1], versionComponents[2], versionComponents[3]);
}

std::string Version::ToString() const
{
    // If the version is unknown, return "unknown"
    if (m_major == -1 && m_minor == -1 && m_build == -1 && m_revision == -1)
    {
        return "unknown";
    }

    return std::to_string(m_major) + "." +
           std::to_string(m_minor) + "." +
           std::to_string(m_build) + 
           (m_revision >= 0 ? "." + std::to_string(m_revision) : "");
}

bool Version::operator<(const Version& other) const
{
    // Compare the major numbers
    if (m_major < other.m_major)
    {
        return true;
    }
    else if (m_major > other.m_major)
    {
        return false;
    }
    else
    {
        // The major numbers are equal, compare the minor numbers
        if (m_minor < other.m_minor)
        {
            return true;
        }
        else if (m_minor > other.m_minor)
        {
            return false;
        }
        else
        {
            // The minor numbers are equal, compare the build numbers
            if (m_build < other.m_build)
            {
                return true;
            }
            else if (m_build > other.m_build)
            {
                return false;
            }
            else
            {
                // The build numbers are also equal, compare the revision numbers
                return m_revision < other.m_revision;
            }
        }
    }
}

std::string Version::GetComponentName(int component)
{
    switch (component)
    {
        case 0: 
            return std::string("Major");

        case 1: 
            return std::string("Minor");

        case 2: 
            return std::string("Build");

        case 3: 
            return std::string("Revision");
        
        default: 
            throw std::invalid_argument("Invalid version component index: " + std::to_string(component));
    }
}