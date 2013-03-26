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

#include <list>
#include <vector>

// ___________________________________________________________________________//

#include "attribute.hpp"
#include "dimension.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace netcdf
{

/**
 * @brief A group is a logical collection of dimensions and attributes
 */
class Group
{
protected:
    /// @brief Dimensions known
    std::vector<Dimension> dimensions_;

    /// @brief Attributes in this group
    std::list<Attribute> attributes_;
public:

    /**
     * @brief Constructor
     */
    Group(): dimensions_(), attributes_()
    {
    }

    /**
     * @brief Default method invoked when a Group is destroyed.
     */
    virtual ~Group()
    {
    }

    /**
     * @brief Get the list of dimensions used in this group.
     *
     * @return Dimension list
     */
    inline std::vector<Dimension> get_dimensions() const
    {
        return dimensions_;
    }

    /**
     * @brief Get the ith dimension.
     *
     * @param index of the dimension.
     * @return requested Dimension
     */
    Dimension const& GetDimension(const int index) const
    {
        return dimensions_[index];
    }

    /**
     * @brief Find the index of the named Dimension.
     *
     * @param name of the dimension
     *
     * @return the index of the named Dimension, or -1 if not found.
     */
    inline int FindDimensionIndex(const std::string& name) const
    {
        for (unsigned int ix = 0; ix < dimensions_.size(); ++ix)
        {
            if (dimensions_[ix].get_name() == name)
                return ix;
        }
        return -1;
    }

    /**
     * @brief Returns the set of attributes for this variable.
     *
     * @return Attribute list
     */
    inline std::list<Attribute> get_attributes() const
    {
        return attributes_;
    }

    /**
     * @brief Find an Attribute by name.
     *
     * @param name of the attribute
     *
     * @return the Attribute or Attribute::MISSING if not found
     */
    Attribute const& FindAttribute(const std::string& name) const;

    /**
     * @brief Find an Attribute by name, ignoring the case
     *
     * @param name of the attribute
     *
     * @return the attribute or Attribute::MISSING if not found
     */
    Attribute const& FindAttributeIgnoreCase(const std::string& name) const;
};

} // namespace netcdf
} // namespace lagrangian
