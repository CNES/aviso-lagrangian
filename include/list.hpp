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

namespace lagrangian
{

/**
 * @brief List which support indexing
 */
template<class T>
class List
{
private:
    std::list<T> items_;
    std::vector<typename std::list<T>::iterator> it_;

public:

    /**
     * @brief Default constructor
     */
    List() :
            items_(), it_()
    {
    }

    /**
     * @brief Returns the number of elements in the list.
     *
     * @return The number of elements in the list
     */
    inline std::size_t Size() const
    {
        return items_.size();
    }

    /**
     * @brief Add data to the end of the list
     *
     * @param item Data to be added
     */
    inline void PushBack(const T& item)
    {
        items_.push_back(item);
        it_.push_back(--(items_.end()));
    }

    /**
     * @brief Returns a reference to the element at position n in the list
     *
     * @param n Position of an element in the list.
     * @return The element at the specified position in the list
     */
    T& operator[](std::size_t n)
    {
        return *it_[n];
    }

    /**
     * @brief Removes from the list all the elements for which predicate
     * returns true
     *
     * @param predicate Unary predicate that, taking a value of the same type
     * as those contained in the list object, returns true for those values
     * to be removed from the list, and false for those remaining.
     */
    template<typename Predicate>
    void Erase(Predicate predicate)
    {
        typename std::list<T>::iterator it = items_.begin();
        typename std::list<T>::iterator last = items_.end();
        int ix = 0;

        while (it != last)
        {
            if (predicate(*it))
                it = items_.erase(it);
            else
            {
                it_[ix++] = it;
                ++it;
            }
        }
        it_.resize(ix);
    }
};

} // namespace lagrangian
