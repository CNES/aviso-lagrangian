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

#include <boost/thread.hpp>

// ___________________________________________________________________________//

#include "map.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace map
{

void FiniteLyapunovExponents::Initialize(
        lagrangian::FiniteLyapunovExponents& fle)
{
    for (int ix = 0; ix < map_.get_nx(); ++ix)
    {
        for (int iy = 0; iy < map_.get_ny(); ++iy)
        {
            Triplet t = fle.SetInitialPoint(map_.GetXValue(ix),
                    map_.GetYValue(iy));
            map_.SetItem(ix, iy, t);
            indexes_.PushBack(Index(ix, iy));
        }
    }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::Initialize(
        lagrangian::FiniteLyapunovExponents& fle,
        lagrangian::reader::Netcdf& reader)
{
    CellProperties cell;

    for (int ix = 0; ix < map_.get_nx(); ++ix)
    {
        for (int iy = 0; iy < map_.get_ny(); ++iy)
        {
            Triplet t = fle.SetInitialPoint(map_.GetXValue(ix),
                    map_.GetYValue(iy));

            if (std::isnan(reader.Interpolate(map_.GetXValue(ix),
                    map_.GetYValue(iy),
                    std::numeric_limits<double>::quiet_NaN(),
                    cell)))
                t.set_completed();
            else
                indexes_.PushBack(Index(ix, iy));
            map_.SetItem(ix, iy, t);
        }
    }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::ComputeHt(Arguments* args,
        lagrangian::FiniteLyapunovExponents& fle,
        Iterator& it)
{
    // Creating an object containing the properties of the interpolation
    CellProperties cell;

    for (int iz = args->i_start; iz < args->i_stop; ++iz)
    {
        const int ix = indexes_[iz].get_i();
        const int iy = indexes_[iz].get_j();

        Triplet& t = map_.GetItem(ix, iy);

        if (!fle.Compute(it, t, cell))
        {
            map_.SetItem(ix, iy, Triplet::MISSING());
        }
        else
        {
            if (fle.Separation(t))
            {
                t.set_completed();
            }
            map_.SetItem(ix, iy, t);
        }
    }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::Compute(lagrangian::FiniteLyapunovExponents& fle)
{
    Iterator it = fle.GetIterator();
    boost::thread_group threads;
    std::vector<Arguments> args(num_threads_);

    // Number of cells to process
    double items = map_.get_nx() * map_.get_ny();

    while (it.GoAfter())
    {
        fle.Fetch(it());

        std::string date = JulianDay(JulianDay::FromUnixTime(it())).ToString(
                "%Y-%m-%d %H:%M:%S");

        Debug(str(boost::format("Start time step %s (%d cells)")
                % date % indexes_.Size()));

        for (int ix = 0; ix < num_threads_; ++ix)
        {
            args[ix].i_start = (ix * indexes_.Size()) / num_threads_;
            args[ix].i_stop = ((ix + 1) * indexes_.Size()) / num_threads_;

            threads.create_thread(
                    boost::bind(
                            &lagrangian::map::FiniteLyapunovExponents::ComputeHt,
                            this, &args[ix], fle, it));
        }

        threads.join_all();

        // Removing cells that are completed
        indexes_.Erase(boost::bind(&FiniteLyapunovExponents::Completed, this, _1));

        Debug(str(boost::format("Close time step %s (%.02f%% completed)")
                % date % ((items - indexes_.Size()) / items * 100)));

        ++it;
    }
}

} // namespace map
} // namespace lagrangian
