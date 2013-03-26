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

#include "axis.hpp"
#include "reader_factory.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

/**
 * @brief Management of a time series consisting of a list of files.
 */
class FileList
{
private:
    Axis axis_;
    std::vector<std::string> filenames_;
    bool same_coordinates_;

public:

    /**
     * @brief Create a new instance of FileList.
     *
     * @param filenames List of files constituting the time series.
     * @param varname Name the variable containing data to be processed.
     * @param reader Instance of an object implementing the class Reader.
     */
    FileList(const std::vector<std::string>& filenames,
            const std::string& varname,
            Reader* const reader);

    /**
     * @brief Given a date expressed as a number of seconds elapsed since
     * 1970, find elements around it. This mean that
     * @code
     * dates[i0] <= date < dates[i1]
     * @endcode
     *
     * @param date Date request
     * @param i0
     * @param i1
     *
     * @throw std::out_of_range if the date requested is not included in the
     * time series.
     */
    inline void FindIndexes(const double date, int& i0, int& i1)
    {
        if (axis_.FindIndexes(date, i0, i1) == false)
        {
            throw std::out_of_range("date out of range");
        }
    }

    /**
     * @brief Get the number of filename in this time series
     *
     * @return number of filenames
     */
    inline int GetNumElements() const
    {
        return axis_.GetNumElements();
    }

    /**
     * @brief Get the ith filename.
     *
     * @param index which filename. Between 0 and GetNumElements()-1 inclusive
     *
     * @return filename
     *
     * @throw std::out_of_range if index is out of range
     */
    inline std::string& GetItem(const int index)
    {
        return filenames_.at(index);
    }

    /**
     * @brief Get the ith date
     *
     * @param index which date
     *
     * @return date in JulianDay
     */
    inline double GetDate(const int index) const
    {
        return axis_.GetCoordinateValue(index);
    }

    /**
     * @brief Returns true if the file list have the same spatial coordinates.
     *
     * @return false if the file list have different spatial coordinates.
     */
    inline bool same_coordinates() const
    {
        return same_coordinates_;
    }
};

// ___________________________________________________________________________//

/**
 * @brief Spatio-temporal interpolation of a series of grids.
 *
 * The series is described by a list of files containing only one date per
 * file.
 */
class TimeSerie
{
private:
    std::vector<Reader*> readers_;
    FileList* time_serie_;
    int first_index_, last_index_;
    std::string varname_;
    std::string unit_;
    bool same_coordinates_;
    reader::Factory::Type type_;
    std::map<std::string, int> files_;

    // Load new files in memory if necessary.
    void Load(int ix0, const int ix1);

public:

    /**
     * @brief Loads the data necessary for the interpolation of the values in
     * the interval [begin, end].
     *
     * @param begin Date of the first measurement to compute in number of
     *  seconds elapsed since 1/1/1970)
     * @param end Date of the last measurement to compute in number of
     *  seconds elapsed since 1/1/1970)
     */
    void Load(const double t0, const double t1);

    /**
     * @brief Create a new instance of TimeSerie.
     *
     * @param filenames List of files constituting the time series.
     * @param varname Name the variable containing data to be processed.
     * @param unit Unit of data required by the user. If the parameter is
     * undefined or contains an empty string, the object will not do unit
     * conversion(i.e. the unit of the interpolated value is the unit of the
     * file)
     * @param reader Instance of an object implementing the class Reader. By
     * default the class uses the reader of NetCDF grids.
     */
    TimeSerie(const std::vector<std::string>& filenames,
            const std::string& varname,
            const std::string& unit = "",
            const reader::Factory::Type type = reader::Factory::kNetCDF);

    /**
     * @brief Default method invoked when a TimeSerie is destroyed.
     */
    ~TimeSerie()
    {
        for (std::vector<Reader*>::iterator it = readers_.begin(); it
                != readers_.end(); ++it)
            delete *it;
        delete time_serie_;
    }

    /**
     * @brief Computes the value of point (x, y, t) in the series.
     *
     * @param date t (in number of seconds elapsed since 1970-1-1
     * 00:00:00.0+00:00)
     * @param longitude x
     * @param latitude y
     *
     * @return the interpolated value
     *
     */
    double Interpolate(const double date,
            double& longitude,
            const double latitude,
            Coordinates& coordinates=Coordinates::UNDEF());

    /**
     * @brief Returns the first date of the time series.
     *
     * @returns the first date
     */
    inline double GetFirstDate() const
    {
        return time_serie_->GetDate(0);
    }

    /**
     * @brief Returns the last date of the time series.
     *
     * @returns the last date
     */
    inline double GetLastDate() const
    {
        return time_serie_->GetDate(time_serie_->GetNumElements() - 1);
    }

    /**
     * @brief Get the number of items in this time series
     *
     * @return number of filenames
     */
    inline int GetNumElements() const
    {
        return time_serie_->GetNumElements();
    }

    /**
     * @brief Returns true if the time series data have the same spatial
     * coordinates.
     *
     * @return false if the time series data have different spatial
     * coordinates.
     */
    inline bool same_coordinates() const
    {
        return same_coordinates_;
    }
};

}
