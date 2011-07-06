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

#ifndef TIME_SERIE_H_
#define TIME_SERIE_H_

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
     * @brief Given a date expressed as JulianDay, find the file name closest to
     * the date requested.
     *
     * @param date requested
     *
     * @return index of the filename closest to the date
     *
     * @throw std::out_of_range if the date requested is not included in the
     * time series.
     */
    int FindIndex(const double date)
    {
        int index = axis_.FindIndex(date);

        if (index == -1)
        {
            throw std::out_of_range("date out of range");
        }
        return index;
    }

    /**
     * @brief Get the number of filename in this time series
     *
     * @return number of filenames
     */
    int GetNumElements() const
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
    std::string& GetItem(const int index)
    {
        if (index < 0 || index > GetNumElements() - 1)
        {
            throw std::out_of_range("index out of range");
        }
        return filenames_[index];
    }

    /**
     * @brief Get the ith date
     *
     * @param index which date
     *
     * @return date in JulianDay
     */
    double GetDate(const int index) const
    {
        return axis_.GetCoordinateValue(index);
    }
};

// ___________________________________________________________________________//

/**
 * @brief Spatio-temporal interpolation of a series of grids.
 *
 * The series is described by a list of files containing only one date per file.
 */
class TimeSerie
{
private:
    std::vector<Reader*> readers_;
    FileList* time_serie_;
    int first_index_, last_index_;
    double last_date_;
    std::string varname_;
    std::string unit_;

    // Load new files in memory if necessary.
    void Load(int& ix0, const int ix1);

public:

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
     * @param n Number of grids to load into memory
     */
    TimeSerie(const std::vector<std::string>& filenames,
            const std::string& varname,
            const std::string& unit = "",
            const reader::Factory::Type type = reader::Factory::kNetCDF,
            const int n = 4);

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
            const double longitude,
            const double latitude);

    /**
     * @brief Returns the first date of the time series.
     *
     * @returns the first date
     */
    double GetFirstDate() const
    {
        return time_serie_->GetDate(0);
    }

    /**
     * @brief Returns the last date of the time series.
     *
     * @returns the last date
     */
    double GetLastDate() const
    {
        return time_serie_->GetDate(time_serie_->GetNumElements() - 1);
    }

    /**
     * @brief Get the number of items in this time series
     *
     * @return number of filenames
     */
    int GetNumElements() const
    {
        return time_serie_->GetNumElements();
    }
};

}

#endif /* TIME_SERIE_H_ */
