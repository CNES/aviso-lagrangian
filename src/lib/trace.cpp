/*
 This file is part of lagrangian library.

 lagrangian is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 lagrangian is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
 */

// ___________________________________________________________________________//

#include "julian_day.hpp"
#include "trace.h"

// ___________________________________________________________________________//

namespace lagrangian
{

/**
 * @brief Configuration manager
 */
class Configuration
{
private:
    bool verbose_;

public:

    /**
     * @brief Default constructor
     */
    Configuration() :
        verbose_(false)
    {

    }

    /**
     * @brief Display a debug message
     *
     * @param message Message to display
     */
    inline void Debug(std::string const& message)
    {
        if (verbose_)
        {
            JulianDay now;
            std::cerr << str(boost::format("[ %s ] %s") %
                    now.ToString("%Y-%m-%d %H:%M:%S.%f") %
                    message) << std::endl;
            std::cerr.flush();
        }
    }

    /**
     * @brief Enable or disable verbose mode
     *
     * @param value Value to enable (true) or disable (false) verbose mode
     */
    inline void set_verbose(const bool value)
    {
        verbose_ = value;
    }

    /**
     * @brief Get the version number
     *
     * @return The version number
     */
    static std::string Version()
    {
        return "__VERSION__";
    }
};

static Configuration g_configuration;

void SetVerbose(const bool value)
{
    g_configuration.set_verbose(value);
}

void Debug(std::string const& message)
{
    g_configuration.Debug(message);
}

std::string Version()
{
    return g_configuration.Version();
}

}
