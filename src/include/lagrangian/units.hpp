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
#include <sstream>

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
    /**
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

}// namespace units

// ___________________________________________________________________________//

/**
 * @brief Handles a unit converter
 */
class UnitConverter
{
public:

    /**
     * Default constructor
     *
     * @param offset the numeric offset
     * @param scale the numeric scale factor
     */
    UnitConverter(const double offset = 0, const double scale = 1) :
            offset_(offset), scale_(scale)
    {

    }

    /**
     * Tests if the converter is null
     *
     * @return true if the converter is null otherwise false
     */
    inline bool IsNull() const
    {
        return offset_ == 0 && scale_ == 1;
    }

    /**
     * Converts a value value and returns the new value
     *
     * @param value value to convert
     */
    template<class T>
    inline void Convert(std::vector<T>& values) const
    {
        if (offset_ == 0 && scale_ == 1)
            return;

        for (auto& item: values)
            item = item * scale_ + offset_;
    }

    template<class T>
    inline void Convert(T& value) const
    {
        value = value * scale_ + offset_;
    }

    /**
     * @return A string representation of the converter
     */
    std::string ToString() const
    {
        std::stringstream ss;
        ss.precision(6);
        ss << std::fixed << "x * " << scale_ << " + " << offset_;
        return ss.str();
    }

private:
    double offset_;
    double scale_;
};

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
     * @return The converter computed
     */
    static UnitConverter GetConverter(const std::string& from,
            const std::string& to);

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
     * @return If unit represents a time
     */
    static bool IsTime(const std::string& unit)
    {
        return AreConvertible(unit, "seconds since 1970-01-01 00:00:00");
    }
};

} //namespace lagrangian
