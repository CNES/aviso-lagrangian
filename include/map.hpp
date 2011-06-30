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

#ifndef MAP_HPP_
#define MAP_HPP_

#include "integration.hpp"

namespace lagrangian
{

class MapProperties
{
protected:
    int nx_;
    int ny_;
    double x_min_;
    double y_min_;
    double step_;
public:
    MapProperties(const int nx,
            const int ny,
            const double x_min,
            const double y_min,
            const double step) :
        nx_(nx), ny_(ny), x_min_(x_min), y_min_(y_min), step_(step)
    {
    }
    inline double GetXValue(const int ix) const
    {
        return x_min_ + ix * step_;
    }
    inline double GetYValue(const int iy) const
    {
        return y_min_ + iy * step_;
    }
    inline int get_nx() const
    {
        return nx_;
    }
    inline int get_ny() const
    {
        return ny_;
    }
    inline double get_step() const
    {
        return step_;
    }
    inline double get_x_min() const
    {
        return x_min_;
    }
    inline double get_y_min() const
    {
        return y_min_;
    }
};

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
    inline T GetItem(const int ix, const int iy) const
    {
        return grid_[ix * get_ny() + iy];
    }
};

namespace map
{

class FiniteLyapunovExponents
{
protected:
    Map<Triplet> map_;

public:
    FiniteLyapunovExponents(const int nx,
            const int ny,
            const double x_min,
            const double y_min,
            const double step) :
        map_(nx, ny, x_min, y_min, step)

    {
    }

    void Compute(lagrangian::FiniteLyapunovExponents& fle)
    {
        for (int ix = 0; ix < map_.get_nx(); ++ix)
        {
            for (int iy = 0; iy < map_.get_ny(); ++iy)
            {
                Triplet t = fle.SetInitialPoint(map_.GetXValue(ix),
                        map_.GetYValue(iy));
                map_.SetItem(ix, iy, t);
            }
        }

        Iterator it = fle.GetIterator();

        while (it.GoAfter())
        {
            for (int ix = 0; ix < map_.get_nx(); ++ix)
            {
                for (int iy = 0; iy < map_.get_ny(); ++iy)
                {
                    Triplet t = map_.GetItem(ix, iy);
                    if (!t.IsMissing())
                    {
                        if (!fle.Compute(it, t))
                            map_.SetItem(ix, iy, Triplet::MISSING());
                        else
                            map_.SetItem(ix, iy, t);
                    }
                }
            }
            ++it;
        }
    }
};

}

class MapOfFiniteLyapunovExponents: public map::FiniteLyapunovExponents
{
private:
    typedef double
    (lagrangian::FiniteLyapunovExponents::*GetExponent)() const;

    Map<double> GetMapOfExponents(const double nan,
            const Iterator& it,
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
                Triplet t = map_.GetItem(ix, iy);
                if (t.IsMissing())
                {
                    result.SetItem(ix, iy, nan);
                }
                else
                {
                    fle.Exponents(it, t);
                    result.SetItem(ix, iy, (fle.*pGetExponent)());
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
            const Iterator& it,
            lagrangian::FiniteLyapunovExponents& fle)
    {
        return GetMapOfExponents(nan,
                it,
                fle,
                &lagrangian::FiniteLyapunovExponents::get_lambda1);
    }

    Map<double> GetMapOfLambda2(const double nan,
            const Iterator& it,
            lagrangian::FiniteLyapunovExponents& fle)
    {
        return GetMapOfExponents(nan,
                it,
                fle,
                &lagrangian::FiniteLyapunovExponents::get_lambda2);
    }

    Map<double> GetMapOfTheta1(const double nan,
            const Iterator& it,
            lagrangian::FiniteLyapunovExponents& fle)
    {
        return GetMapOfExponents(nan,
                it,
                fle,
                &lagrangian::FiniteLyapunovExponents::get_theta1);
    }

    Map<double> GetMapOfTheta2(const double nan,
            const Iterator& it,
            lagrangian::FiniteLyapunovExponents& fle)
    {
        return GetMapOfExponents(nan,
                it,
                fle,
                &lagrangian::FiniteLyapunovExponents::get_theta2);
    }

};

}

#endif /* MAP_HPP_ */
