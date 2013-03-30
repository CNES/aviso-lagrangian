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

#include <cstdlib>

// ___________________________________________________________________________//

#include "integration.hpp"
#include "netcdf_reader.hpp"
#include "trace.h"

// ___________________________________________________________________________//

namespace std
{
    using ::getenv;
}

// ___________________________________________________________________________//

namespace lagrangian
{

/**
 * Properties of a regular grid
 */
class MapProperties
{
protected:
    double x_min_;
    double y_min_;
    double step_;
    int nx_;
    int ny_;

public:

    /**
     * @brief Default constructor
     *
     * @param nx Number of longitudes
     * @param ny Number of latitudes
     * @param x_min Minimal longitude
     * @param y_min Minimal latitude
     * @param step Step between two consecutive longitudes and latitudes
     */
    MapProperties(const int nx,
            const int ny,
            const double x_min,
            const double y_min,
            const double step) :
        x_min_(x_min), y_min_(y_min), step_(step), nx_(nx), ny_(ny)
    {
    }

    /**
     * @brief Default method invoked when a MapProperties is destroyed.
     */
    virtual ~MapProperties()
    {
    }

    /**
     * @brief Get the longitude value
     *
     * @param ix Index of the longitude in the grid
     * @return The longitude
     */
    inline double GetXValue(const int ix) const
    {
        return x_min_ + ix * step_;
    }

    /**
     * @brief Get the latitude value
     *
     * @param ix Index of the latitude in the grid
     * @return The longitude
     */
    inline double GetYValue(const int iy) const
    {
        return y_min_ + iy * step_;
    }

    /**
     * @brief Get the number of longitudes in the grid
     *
     * @return The number of longitudes
     */
    inline int get_nx() const
    {
        return nx_;
    }

    /**
     * @brief Get the number of latitudes in the grid
     *
     * @return The number of latitudes
     */
    inline int get_ny() const
    {
        return ny_;
    }

    /**
     * @brief Get the step between two consecutive longitudes and latitudes
     *
     * @return The step
     */
    inline double get_step() const
    {
        return step_;
    }

    /**
     * @brief Get the minimal longitude
     *
     * @return The minimal longitude
     */
    inline double get_x_min() const
    {
        return x_min_;
    }

    /**
     * @brief Get the minimal latitude
     *
     * @return The minimal latitude
     */
    inline double get_y_min() const
    {
        return y_min_;
    }
};

// ___________________________________________________________________________//

template<class T> class Map: public MapProperties
{
private:
    std::vector<T> grid_;
public:
    Map(const int nx,
            const int ny,
            const double x_min,
            const double y_min,
            const double step) :
        MapProperties(nx, ny, x_min, y_min, step),
                grid_(std::vector<T>(nx * ny))
    {
    }
    inline void SetItem(const int ix, const int iy, const T& item)
    {
        grid_[ix * get_ny() + iy] = item;
    }
    inline T& GetItem(const int ix, const int iy)
    {
        return grid_[ix * get_ny() + iy];
    }
};

// ___________________________________________________________________________//

namespace map
{

class FiniteLyapunovExponents
{
private:
    struct Arguments
    {
        int i_start;
        int i_stop;
        double completed;
    };
    void ComputeHt(Arguments* args,
            lagrangian::FiniteLyapunovExponents& fle,
            Iterator& it);
    int num_threads_;

protected:
    Map<Triplet> map_;

public:
    FiniteLyapunovExponents(const int nx,
            const int ny,
            const double x_min,
            const double y_min,
            const double step) :
                num_threads_(1), map_(nx, ny, x_min, y_min, step)

    {
        char* omp_num_threads = std::getenv("OMP_NUM_THREADS");
        if( omp_num_threads )
        {
            try
            {
                num_threads_ = boost::lexical_cast<int>(omp_num_threads);
            }
            catch (boost::bad_lexical_cast& e)
            {
                throw std::runtime_error(
                        std::string("Invalid value for OMP_NUM_THREADS: ")
                            + omp_num_threads);
            }
        }
        Debug(str(boost::format("Uses %d threads") % num_threads_));
    }

    virtual ~FiniteLyapunovExponents()
    {
    }

    void Initialize(lagrangian::FiniteLyapunovExponents& fle);

    void Initialize(lagrangian::FiniteLyapunovExponents& fle,
            lagrangian::reader::Netcdf& reader);

    void Compute(lagrangian::FiniteLyapunovExponents& fle);
};

}

// ___________________________________________________________________________//

class MapOfFiniteLyapunovExponents: public map::FiniteLyapunovExponents
{
private:
    typedef double
    (lagrangian::FiniteLyapunovExponents::*GetExponent)() const;

    Map<double> GetMapOfExponents(const double nan,
            lagrangian::FiniteLyapunovExponents& fle,
            GetExponent pGetExponent)
    {
        Map<double> result(map_.get_nx(),
                map_.get_ny(),
                map_.get_x_min(),
                map_.get_y_min(),
                map_.get_step());

        for (int ix = 0; ix < map_.get_nx(); ++ix)
        {
            for (int iy = 0; iy < map_.get_ny(); ++iy)
            {
                Triplet& t = map_.GetItem(ix, iy);
                if (t.IsMissing())
                {
                    result.SetItem(ix, iy, nan);
                }
                else if( !t.get_completed() && fle.get_mode() ==
                        lagrangian::FiniteLyapunovExponents::kFSLE )
                {
                    result.SetItem(ix, iy, 0);
                }
                else
                {
                    double exponent = fle.Exponents(t)
                        ? (fle.*pGetExponent)()
                        : std::numeric_limits<double>::quiet_NaN();
                    result.SetItem(ix, iy, exponent);
                }
            }
        }
        return result;
    }
public:
    MapOfFiniteLyapunovExponents(const int nx,
            const int ny,
            const double x_min,
            const double y_min,
            const double step) :
        map::FiniteLyapunovExponents(nx, ny, x_min, y_min, step)
    {
    }

    Map<double> GetMapOfLambda1(const double nan,
            lagrangian::FiniteLyapunovExponents& fle)
    {
        return GetMapOfExponents(nan,
                fle,
                &lagrangian::FiniteLyapunovExponents::get_lambda1);
    }

    Map<double> GetMapOfLambda2(const double nan,
            lagrangian::FiniteLyapunovExponents& fle)
    {
        return GetMapOfExponents(nan,
                fle,
                &lagrangian::FiniteLyapunovExponents::get_lambda2);
    }

    Map<double> GetMapOfTheta1(const double nan,
            lagrangian::FiniteLyapunovExponents& fle)
    {
        return GetMapOfExponents(nan,
                fle,
                &lagrangian::FiniteLyapunovExponents::get_theta1);
    }

    Map<double> GetMapOfTheta2(const double nan,
            lagrangian::FiniteLyapunovExponents& fle)
    {
        return GetMapOfExponents(nan,
                fle,
                &lagrangian::FiniteLyapunovExponents::get_theta2);
    }
};

}
