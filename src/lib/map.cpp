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

void FiniteLyapunovExponents::Initialize(lagrangian::FiniteLyapunovExponents& fle,
        const lagrangian::FiniteLyapunovExponents::Stencil stencil)
{
    for (int ix = 0; ix < map_.get_nx(); ++ix)
    {
        for (int iy = 0; iy < map_.get_ny(); ++iy)
        {
            Position t = fle.SetInitialPoint(map_.GetXValue(ix),
                    map_.GetYValue(iy), stencil);
            map_.SetItem(ix, iy, t);
            indexes_.push_back(Index(ix, iy));
        }
    }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::Initialize(lagrangian::FiniteLyapunovExponents& fle,
        lagrangian::reader::Netcdf& reader,
        const lagrangian::FiniteLyapunovExponents::Stencil stencil)
{
    CellProperties cell;

    for (int ix = 0; ix < map_.get_nx(); ++ix)
    {
        for (int iy = 0; iy < map_.get_ny(); ++iy)
        {
            Position t = fle.SetInitialPoint(map_.GetXValue(ix),
                    map_.GetYValue(iy), stencil);

            if (std::isnan(
                    reader.Interpolate(map_.GetXValue(ix), map_.GetYValue(iy),
                            std::numeric_limits<double>::quiet_NaN(), cell)))
                t.set_completed();
            else
                indexes_.push_back(Index(ix, iy));
            map_.SetItem(ix, iy, t);
        }
    }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::ComputeHt(Splitter<Index>& splitter,
        lagrangian::FiniteLyapunovExponents& fle,
        Iterator& it)
{
    // Creating an object containing the properties of the interpolation
    CellProperties cell;

    std::list<Index>::iterator first = splitter.begin();
    while (first != splitter.end())
    {
        const int ix = first->get_i();
        const int iy = first->get_j();

        Position& t = map_.GetItem(ix, iy);

        if (!fle.Compute(it, t, cell))
        {
            map_.SetItem(ix, iy, Position::MISSING());
        }
        else
        {
            if (fle.Separation(t))
            {
                t.set_completed();
            }
            map_.SetItem(ix, iy, t);
        }
        ++first;
    }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::Compute(lagrangian::FiniteLyapunovExponents& fle)
{
    Iterator it = fle.GetIterator();
    boost::thread_group threads;

    // Number of cells to process
    double items = map_.get_nx() * map_.get_ny();

    std::list<Splitter<Index> > splitters = indexes_.Split(num_threads_);

    while (it.GoAfter())
    {
        fle.Fetch(it());

        std::string date = JulianDay(JulianDay::FromUnixTime(it())).ToString(
                "%Y-%m-%d %H:%M:%S");

        Debug(
                str(
                        boost::format("Start time step %s (%d cells)") % date
                                % indexes_.size()));

        for (std::list<lagrangian::Splitter<Index> >::iterator its =
                splitters.begin(); its != splitters.end(); ++its)
        {
            threads.create_thread(
                    boost::bind(
                            &lagrangian::map::FiniteLyapunovExponents::ComputeHt,
                            this, *its, fle, it));
        }

        threads.join_all();

        // Removing cells that are completed
        splitters = indexes_.Erase(
                boost::bind(&FiniteLyapunovExponents::Completed, this, _1),
                num_threads_);

        Debug(
                str(
                        boost::format("Close time step %s (%.02f%% completed)")
                                % date
                                % ((items - indexes_.size()) / items * 100)));

        ++it;
    }
}

} // namespace map
} // namespace lagrangian
