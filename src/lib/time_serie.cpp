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

// ___________________________________________________________________________//

namespace lagrangian
{

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

    // For all files, find the time to create an associative array: date,
    // filename.
    for (its = filenames.begin(); its != filenames.end(); ++its)
    {
        reader->Open(*its);
        files.push_back(std::make_pair(reader->GetJulianDay(varname).ToUnixTime(),
                *its));
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
        const int n,
        const reader::Factory::Type type) :
    first_index_(-1), last_index_(-1), varname_(varname), unit_(unit)
{
    readers_.resize(n);

    for (int ix = 0; ix < n; ++ix)
        readers_[ix] = reader::Factory::NewReader(type);

    // Create the time series
    time_serie_ = new FileList(filenames, varname_, readers_[0]);
}

// ___________________________________________________________________________//

double TimeSerie::Interpolate(const double date,
        const double longitude,
        const double latitude)
{
    int ix0 = time_serie_->FindIndex(date);
    int ix1 = ix0 == time_serie_->GetNumElements() - 1 ? ix0 - 1 : ix0 + 1;

    // Should we load new data into memory ?
    if (ix0 < first_index_ || ix0 > last_index_ || ix1 < first_index_ || ix1
            > last_index_)
    {
        std::vector<int> indexes;
        int start = 0;

        // Keep the last file into memory
        if (ix0 > 0)
            --ix0;

        indexes.push_back(ix0);

        for (int ix = 1; ix < static_cast<int> (readers_.size()); ++ix)
        {
            if (indexes[start] + ix < time_serie_->GetNumElements())
            {
                indexes.push_back(indexes[start] + ix);
            }
            else
            {
                indexes.insert(indexes.begin(), indexes[0] - 1);
                start += 1;
            }
        }

        first_index_ = ix0;
        last_index_ = ix0 + readers_.size() - 1;

        for (int ix = 0; ix < static_cast<int> (readers_.size()); ++ix)
        {
            readers_[ix]->Open(time_serie_->GetItem(indexes[ix]));
            readers_[ix]->Load(varname_, unit_);
        }
    }

    const double x0 = time_serie_->GetDate(ix0);
    const double x1 = time_serie_->GetDate(ix1);

    ix0 = ix0 - first_index_;
    ix1 = ix1 - first_index_;

    const double dx = 1 / (x1 - x0);

    const double y0 = readers_[ix0]->Interpolate(longitude, latitude);
    const double y1 = readers_[ix1]->Interpolate(longitude, latitude);

    const double w0 = (x1 - date) * dx;
    const double w1 = (date - x0) * dx;

    return (w0 * y0 + w1 * y1) / (w0 + w1);
}

}
