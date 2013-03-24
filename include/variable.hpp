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

#include <string>
#include <vector>

// ___________________________________________________________________________//

#include "scale_missing.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace netcdf
{

/**
 * @brief A Variable is a logical container for data. It has a set of
 * Dimensions that define its array shape, and optionally a set of
 * Attributes.
 */
class Variable: public Group
{
private:
    std::string name_;
    std::vector<long int> shape_;

    NcVar* ncvar_;

    ScaleMissing scale_missing_;

    Variable() :
        name_(""), shape_(), ncvar_(NULL), scale_missing_()
    {
    }

public:
    /**
     * @brief Get the name of this variable.
     *
     * @return variable name
     */
    inline std::string get_name() const
    {
        return name_;
    }

    /**
     * @brief Get the shape: length of Variable in each dimension.
     * @return array whose length is the rank of this variable.
     */
    inline std::vector<long int> get_shape() const
    {
        return shape_;
    }

    /**
     * @brief Get the size of the ith dimension
     *
     * @param index which dimension
     *
     * @return size of the ith dimension
     */
    inline long int get_shape(const int index) const
    {
        return shape_[index];
    }

    /**
     * @brief Get the total number of elements in the Variable.
     *
     * @return total number of elements in the Variable.
     */
    inline long GetSize() const
    {
        long result = 1;
        std::vector<long int>::const_iterator it;

        for (it = shape_.begin(); it < shape_.end(); ++it)
            result *= *it;

        return result;
    }

    /**
     * @brief Get the number of dimensions of the Variable.
     *
     * @return the rank
     */
    inline long int GetRank() const
    {
        return shape_.size();
    }

    /**
     * @brief Whether this is a scalar Variable (rank == 0).
     *
     * @return true if Variable has rank 0
     */
    inline bool IsScalar() const
    {
        return GetRank() == 0;
    }

    /**
     * @brief Get the description of the Variable.
     * Default is to use "long_name" attribute value. If not exist, look
     * for "description", "title", or "standard_name" attribute value (in
     * that order).
     * @param description found
     *
     * @return true if a description exists, otherwise false
     */
    bool GetDescription(std::string& description) const;

    /**
     * @brief Get the unit string for the variable
     * Looks for the "units" attribute value
     *
     * @param units unit found
     *
     * @return true if units attribute exists otherwise false
     */
    bool GetUnitsString(std::string& units) const;

    /**
     * @brief Create a variable.
     *
     * @param var an instance of NcVar object
     */
    Variable(NcVar* var);

    /**
     * @brief Calculate if this is a classic coordinate variable: has same name
     * as its first dimension
     */
    inline bool IsCoordinateVariable() const
    {
        if (GetRank() == 1)
        {
            Dimension first_dimension = GetDimension(0);
            if (name_ == first_dimension.get_name())
                return true;
        }
        return false;
    }

    /**
     * @brief compare two variables instances
     *
     * @param a A variable to compare
     * @param b An other variable to compare
     *
     * @return if variables are equals
     */
    inline friend bool operator==(Variable const & a, Variable const & b)
    {
        return a.name_ == b.name_ && a.shape_ == b.shape_;
    }

    /**
     * @brief compare two variables instances
     *
     * @param a A variable to compare
     * @param b An other variable to compare
     *
     * @return if variables are differents
     */
    inline friend bool operator!=(Variable const & a, Variable const & b)
    {
        return a.name_ != b.name_ || a.shape_ != b.shape_;
    }

    /**
     * @brief Read all the data for this variable.
     *
     * @param data read
     */
    void Read(std::vector<double>& data) const;

    /**
     * @brief Read all the data for this variable and convert data to the
     * requested unit.
     *
     * @param data read and converted
     * @param unit used to convert data
     */
    void Read(std::vector<double>& data, const std::string& unit) const;

    /**
     * @brief Represents a missing variable.
     */
    static Variable const MISSING;
};

} // namespace netcdf
} // namespace lagrangian
