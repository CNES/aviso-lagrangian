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
 * @brief Iterators for traversing a sublist
 */
template<class T>
class Splitter
{
private:
    typename std::list<T>::iterator begin_;
    typename std::list<T>::iterator end_;

public:

    /**
     * @brief Default constructor
     *
     * @param begin Start of the sublist
     * @param end End of the sublist
     */
    Splitter(typename std::list<T>::iterator& begin,
        typename std::list<T>::iterator& end) :
            begin_(begin), end_(end)
    {

    }

    /**
     * @brief Start of the sublist
     */
    inline const typename std::list<T>::iterator begin() const
    {
        return begin_;
    }

    /**
     * @brief End of the sublist
     */
    inline const typename std::list<T>::iterator& end() const
    {
        return end_;
    }
};

/**
 * @brief List that can be splitted into n sub-list.
 */
template<class T>
class SplitList: public std::list<T>
{
private:

    /**
     * @brief To split a list into n sublists without deleting items
     * @param value Value to test
     * @return false
     */
    static inline bool predicate(const T& value)
    {
        return false;
    }

public:

    /**
     * @brief Default constructor
     */
    SplitList() :
            std::list<T>()
    {
    }

    /**
     * @brief Divides the list in sublists
     *
     * @param n_sublist Number of sublist to handle
     */
    inline std::list< Splitter<T> > Split(const int n_sublist)
    {
        return Erase(predicate, n_sublist);
    }

    /**
     * @brief Removes from the list all the elements for which predicate
     * returns true
     *
     * @param predicate Unary predicate that, taking a value of the same type
     * as those contained in the list object, returns true for those values
     * to be removed from the list, and false for those remaining.
     * @param n_sublist Number of sublist to handle
     * @return List of sublist.
     */
    template<typename Predicate>
    std::list< Splitter<T> > Erase(Predicate predicate, const int n_sublist);
};

template<class T> template<typename Predicate>
std::list< Splitter<T> > SplitList<T>::Erase(Predicate predicate, const int n_sublist)
{
    typename std::list<T>::iterator it = this->begin();
    typename std::list<T>::iterator last = this->end();
    typename std::list<T>::iterator first = it;
    std::list< Splitter<T> > splitters;
    int ix = 0;
    int i_sublist = 0;
    int size = this->size();
    int stop = size / n_sublist;

    // For all items
    while (it != last)
    {
        // Deletes the current element if the user wishes
        if (predicate(*it))
        {
            // Resets the first iterator if needed
            it = first == it
                ? first = this->erase(it) : this->erase(it);
        }
        else
        {
            // We need to create a division
            if (ix == stop)
            {
                splitters.push_back(Splitter<T>(first, it));

                // Index of the last item of the next division
                ++i_sublist;
                stop = ((i_sublist + 1) * size) / n_sublist;
                // Sets the first iterator
                first = it;
            }
            ++ix;
            ++it;
        }
    }
    if (first != it)
        splitters.push_back(Splitter<T>(first, it));
    return splitters;
}

} // namespace lagrangian
