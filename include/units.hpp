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

#pragma once

// ___________________________________________________________________________//

#include <stdexcept>
#include <string>
#include <udunits2.h>
#include <vector>

namespace lagrangian
{
namespace units
{
// ___________________________________________________________________________//

/**
 * @brief Exceptions raised by udunits2.
 */

class Exception: public std::runtime_error
{
public:
    /*
     * @brief Default Constructor
     */
    Exception();
    /**
     * @brief Constructs a new instance with a character string describing the
     * error raised by uduntis2
     *
     * @param msg a character string describing the error
     */
    Exception(const std::string& msg) :
        std::runtime_error(msg)
    {
    }
};

// ___________________________________________________________________________//

/**
 * @brief Handles udunits2 resources
 */
class SmartUtSystem
{
private:
    ut_system* system_;
public:

    /**
     * @brief Allocates resources used by uduntits2
     */
    SmartUtSystem()
    {
        if (system_ == NULL)
        {
            ut_set_error_message_handler(&ut_ignore);
            system_ = ut_read_xml(NULL);
            if (ut_get_status() != UT_SUCCESS)
                throw units::Exception("failed to initialize UDUnits2 library");
        }
    }

    /**
     * @brief Frees resources used by udunits2
     */
    ~SmartUtSystem()
    {
        ut_free_system(system_);
    }

    /**
     * @brief Returns the unit-system used
     */
    ut_system* get() const
    {
        return system_;
    }
};

// ___________________________________________________________________________//

} // namespace units

// ___________________________________________________________________________//

/**
 * @brief Provides support for units of physical quantities.
 */
class Units
{
public:
    /**
     * @brief Computes a converter of numeric values in unit "from" to numeric
     * values in unit "to".
     *
     * @param from the unit from which to convert values.
     * @param to the unit to which to convert values.
     * @param offset the numeric offset
     * @param scale the numeric scale factor
     */
    static void GetConverter(const std::string& from,
            const std::string& to,
            double& offset,
            double& scale);

    /**
     * @brief Checks if numeric values in unit "from" are convertible to numeric
     * values in unit "to"
     *
     * @param unit1 a unit.
     * @param unit2 another unit.
     *
     * @return Numeric values can be converted between the units.
     */
    static bool AreConvertible(const std::string& unit1,
            const std::string& unit2);

    /**
     * @brief Checks if numeric value in unit "unit" are convertible to time
     * unit.
     *
     * @param unit a unit.
     *
     * @return Numeric values can be converted to time units.
     */
    static bool IsTime(const std::string& unit)
    {
        return AreConvertible(unit, "seconds since 1970-01-01 00:00:00");
    }

    /**
     * @brief Converts the data from unit @c from to @c to
     *
     * @param from The unit from which to convert values
     * @param to Unit to which to convert values.
     * @param array Data to process
     *
     * @return true if the data has been converted.
     */
    template<class T>
    static bool ConvertData(const std::string& from,
            const std::string& to,
            std::vector<T>& array)
    {
        bool result = false;

        if (from != to)
        {
            double scale, offset;

            GetConverter(from, to, offset, scale);

            if (scale != 1 || offset != 0)
            {
                result = true;

                for (typename std::vector<T>::iterator it = array.begin(); it
                        != array.end(); it++)
                    *it = *it * scale + offset;
            }
        }

        return result;
    }
};

} //namespace lagrangian