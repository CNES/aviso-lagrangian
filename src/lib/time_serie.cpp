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

#include <float.h>
#include <algorithm>

// ___________________________________________________________________________//

#include "time_serie.hpp"
#include "parameter.hpp"
#include "trace.h"

// ___________________________________________________________________________//

namespace lagrangian
{

void TimeSerie::Load(int ix0, const int ix1)
{
    // Should we load new data into memory ?
    if (ix0 < first_index_ || ix0 > last_index_ || ix1 < first_index_
            || ix1 > last_index_)
    {
            first_index_ = ix0;
            last_index_ = ix1;

            std::map<std::string, int>::iterator it;
            std::map<std::string, int> new_files;

            for (int ix = ix0; ix <= ix1; ++ix)
            {
                std::string filename = time_serie_->GetItem(ix);
                int iy = ix - first_index_;

                it = files_.find(filename);

                if(it != files_.end())
                {
                    std::swap(readers_[iy], readers_[it->second]);
                    new_files[it->first] = iy;
                }
                else
                    new_files[filename] = iy;
            }

            for (it = new_files.begin(); it != new_files.end(); ++it)
            {
                if (files_.find(it->first) == files_.end())
                {
                    Debug(str(boost::format("Loading %s from %s") % varname_
                        % it->first));
                    readers_[it->second]->Open(it->first);
                    readers_[it->second]->Load(varname_, unit_);
                }
            }
            files_ = new_files;
    }
}

// ___________________________________________________________________________//

struct SortPredicate
{
    bool operator()(const std::pair<double, std::string>& a,
            const std::pair<double, std::string>& b)
    {
        return a.first < b.first;
    }
};

// ___________________________________________________________________________//

FileList::FileList(const std::vector<std::string>& filenames,
        const std::string& varname,
        Reader* const reader)
{
    std::vector<std::string>::const_iterator its;
    std::vector<std::pair<double, std::string> > files;
    Axis axis_x;
    Axis axis_y;

    same_coordinates_ = true;

    // For all files, find the time to create an associative array: date,
    // filename.
    for (its = filenames.begin(); its != filenames.end(); ++its)
    {
        reader->Open(*its);
        files.push_back(std::make_pair(reader->GetJulianDay(varname).ToUnixTime(),
                *its));
        if (its == filenames.begin())
        {
            axis_x = reader->axis_x();
            axis_y = reader->axis_y();
        }
        else
        {
            if (axis_x != reader->axis_x() or axis_y != reader->axis_y())
                same_coordinates_ = false;
        }
    }

    // Data are sorted according file date
    std::sort(files.begin(), files.end(), SortPredicate());

    std::vector<double> points;
    std::vector<std::pair<double, std::string> >::iterator itp;

    // Creates the axis to obtain a filename from a date.
    for (itp = files.begin(); itp != files.end(); ++itp)
    {
        points.push_back((*itp).first);
        filenames_.push_back((*itp).second);
    }
    axis_ = Axis(points, Axis::kTime);
}

// ___________________________________________________________________________//

TimeSerie::TimeSerie(const std::vector<std::string>& filenames,
        const std::string& varname,
        const std::string& unit,
        const reader::Factory::Type type) :
    first_index_(-1), last_index_(-1), varname_(varname),
    unit_(unit), type_(type)
{
    readers_.resize(2);

    for (size_t ix = 0; ix < readers_.size(); ++ix)
        readers_[ix] = reader::Factory::NewReader(type_);

    // Create the time series
    time_serie_ = new FileList(filenames, varname_, readers_[0]);
    same_coordinates_ = time_serie_->same_coordinates();
}

// ___________________________________________________________________________//

double TimeSerie::Interpolate(const double date,
        double& longitude,
        const double latitude,
        Coordinates& coordinates)
{
    int it0, it1;

    time_serie_->FindIndexes(date, it0, it1);

    const double t0 = time_serie_->GetDate(it0);
    const double t1 = time_serie_->GetDate(it1);

    it0 = it0 - first_index_;
    it1 = it1 - first_index_;

    const double dx = 1 / (t1 - t0);

    const double x0 = readers_[it0]->Interpolate(longitude,
            latitude,
            coordinates);
    const double x1 = readers_[it1]->Interpolate(longitude,
            latitude,
            coordinates);

    const double w0 = (t1 - date) * dx;
    const double w1 = (date - t0) * dx;

    return (w0 * x0 + w1 * x1) / (w0 + w1);
}

// ___________________________________________________________________________//

void TimeSerie::Load(const double t0, const double t1)
{
    int it00, it01, it10, it11;

    time_serie_->FindIndexes(t0, it00, it01);
    time_serie_->FindIndexes(t1, it10, it11);

    it00 = it00 < it10 ? it00: it10;
    it11 = it11 > it01 ? it11: it01;

    size_t required_size = abs(it11 - it00) + 1;

    // The current size of the buffer is not adequate
    if (required_size > readers_.size())
    {
        size_t previous_size = readers_.size();
        readers_.resize(required_size);

        for (size_t ix = previous_size; ix < required_size; ++ix)
            readers_[ix] = reader::Factory::NewReader(type_);
    }

    // Loading the needed data
    Load(it00, it11);
}

}
