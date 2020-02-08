// This file is part of lagrangian library.
//
// lagrangian is free software: you can redistribute it and/or modify
// it under the terms of GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// lagrangian is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of GNU Lesser General Public License
// along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
#include <thread>

// ___________________________________________________________________________//

#include "lagrangian/map.hpp"

// ___________________________________________________________________________//

namespace lagrangian::map {

void FiniteLyapunovExponents::Initialize(
    lagrangian::FiniteLyapunovExponentsIntegration& fle,
    const lagrangian::FiniteLyapunovExponentsIntegration::Stencil stencil) {
  auto spherical_equatorial =
      fle.get_field()->get_coordinates_type() == Field::kSphericalEquatorial;
  for (auto ix = 0; ix < map_.get_nx(); ++ix) {
    for (auto iy = 0; iy < map_.get_ny(); ++iy) {
      // If the user restart initialization, it must release the
      // allocated resources
      auto position = map_.GetItem(ix, iy);
      delete position;

      // Allocate and store the new stencil
      position = fle.SetInitialPoint(map_.GetXValue(ix), map_.GetYValue(iy),
                                     stencil, spherical_equatorial);
      map_.SetItem(ix, iy, position);
      indexes_.push_back(Index(ix, iy));
    }
  }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::Initialize(
    lagrangian::FiniteLyapunovExponentsIntegration& fle,
    const lagrangian::Reader* reader,
    const lagrangian::FiniteLyapunovExponentsIntegration::Stencil stencil) {
  CellProperties cell;
  auto spherical_equatorial =
      fle.get_field()->get_coordinates_type() == Field::kSphericalEquatorial;

  for (auto ix = 0; ix < map_.get_nx(); ++ix) {
    for (auto iy = 0; iy < map_.get_ny(); ++iy) {
      // If the user restart initialization, it must release the
      // allocated resources
      auto position = map_.GetItem(ix, iy);
      delete position;

      // Allocate and store the new stencil
      position = fle.SetInitialPoint(map_.GetXValue(ix), map_.GetYValue(iy),
                                     stencil, spherical_equatorial);

      if (std::isnan(reader->Interpolate(
              map_.GetXValue(ix), map_.GetYValue(iy),
              std::numeric_limits<double>::quiet_NaN(), cell))) {
        position->set_completed();
      } else {
        indexes_.push_back(Index(ix, iy));
      }
      map_.SetItem(ix, iy, position);
    }
  }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::ComputeHt(
    Splitter<Index>& splitter,
    lagrangian::FiniteLyapunovExponentsIntegration& fle, Iterator& it) {
  // Creating an object containing the properties of the interpolation
  CellProperties cell;

  auto first = splitter.begin();
  while (first != splitter.end()) {
    auto ix = first->get_i();
    auto iy = first->get_j();
    auto position = map_.GetItem(ix, iy);

    if (!fle.Compute(it, position, cell)) {
      position->Missing();
    } else {
      if (fle.Separation(position)) {
        position->set_completed();
      }
    }
    ++first;
  }
}

// ___________________________________________________________________________//

void FiniteLyapunovExponents::Compute(
    lagrangian::FiniteLyapunovExponentsIntegration& fle,
    size_t num_threads) {
  auto it = fle.GetIterator();
  std::list<std::thread> threads;

  if(num_threads == 0) {
    num_threads = std::thread::hardware_concurrency();
  }

  // Number of cells to process
  double items = map_.get_nx() * map_.get_ny();
  auto splitters = indexes_.Split(num_threads);

  while (it.GoAfter()) {
    fle.Fetch(it());

    auto date =
        DateTime(DateTime::FromUnixTime(it())).ToString("%Y-%m-%d %H:%M:%S");

    Debug(str(boost::format("Start time step %s (%d cells)") % date %
              indexes_.size()));

    for (auto& item : splitters) {
      threads.emplace_back(
          std::thread(&lagrangian::map::FiniteLyapunovExponents::ComputeHt,
                      this, std::ref(item), std::ref(fle), std::ref(it)));
    }

    for (auto& item : threads) {
      item.join();
    }
    threads.clear();

    // Removing cells that are completed
    splitters = indexes_.Erase(std::bind(&FiniteLyapunovExponents::Completed,
                                         this, std::placeholders::_1),
                               num_threads);

    Debug(str(boost::format("Close time step %s (%.02f%% completed)") % date %
              ((items - indexes_.size()) / items * 100)));

    ++it;
  }
}

}  // namespace lagrangian::map
