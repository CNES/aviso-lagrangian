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

#ifndef INTEGRATION_HPP_
#define INTEGRATION_HPP_

// ___________________________________________________________________________//

#include <boost/date_time.hpp>
#include <boost/algorithm/minmax.hpp>
#include <limits>

// ___________________________________________________________________________//

#include "field.hpp"
#include "julian_day.hpp"
#include "runge_kutta.hpp"

namespace lagrangian
{

class Iterator
{
private:
    double end_;
    double inc_;
    double ix_;
public:
    Iterator(const double begin, const double end, const double inc) :
        end_(end), inc_(begin > end ? -inc : inc), ix_(begin)
    {
    }
    inline bool GoAfter() const
    {
        return inc_ > 0 ? ix_ <= end_ : ix_ >= end_;
    }
    inline void operator++()
    {
        ix_ += inc_;
    }
    inline double operator()() const
    {
        return ix_;
    }
};

// ___________________________________________________________________________//

class Integration
{
protected:
    double start_time_;
    double end_time_;
    double size_of_interval_;
    double min_separation_;

    const Field* const field_;

    RungeKutta rk_;
public:
    Integration(const JulianDay& start_time,
            const JulianDay& end_time,
            const boost::posix_time::time_duration& delta_t,
            const double min_separation,
            const Field* const field) :
        start_time_(start_time.ToUnixTime()), end_time_(end_time.ToUnixTime()),
                size_of_interval_(delta_t.total_seconds()),
                min_separation_(min_separation), field_(field),
                rk_(size_of_interval_, field_)
    {
    }

    Iterator GetIterator() const
    {
        return Iterator(start_time_, end_time_, size_of_interval_);
    }
};

// ___________________________________________________________________________//

class Path: public Integration
{
public:
    Path(const JulianDay& start_time,
            const JulianDay& end_time,
            const boost::posix_time::time_duration& delta_t,
            const Field* const field) :
                Integration(start_time,
                        end_time,
                        delta_t,
                        std::numeric_limits<double>::max(),
                        field)
    {
    }

    inline bool Compute(const Iterator& it,
            const double x0,
            const double y0,
            double& x1,
            double& y1) const
    {
        return rk_.Compute(it(), x0, y0, x1, y1);
    }
};

// ___________________________________________________________________________//

class Triplet
{
private:
    double x0_, x1_, x2_;
    double y0_, y1_, y2_;
public:
    Triplet()
    {
    }
    Triplet(const double x0,
            const double x1,
            const double x2,
            const double y0,
            const double y1,
            const double y2) :
        x0_(x0), x1_(x1), x2_(x2), y0_(y0), y1_(y1), y2_(y2)
    {
    }
    inline double get_x0() const
    {
        return x0_;
    }
    inline double get_x1() const
    {
        return x1_;
    }
    inline double get_x2() const
    {
        return x2_;
    }
    inline double get_y0() const
    {
        return y0_;
    }
    inline double get_y1() const
    {
        return y1_;
    }
    inline double get_y2() const
    {
        return y2_;
    }
    inline bool IsMissing()
    {
        return isnan(x0_) || isnan(x1_) || isnan(x2_) || isnan(y0_)
                || isnan(y1_) || isnan(y2_);
    }
    static Triplet const MISSING()
    {
        return Triplet(std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN());
    }
};

// ___________________________________________________________________________//

class FiniteLyapunovExponents: public Integration
{
private:
    const double delta_;
    double lambda1_;
    double lambda2_;
    double theta1_;
    double theta2_;
    double f2_;

public:
    FiniteLyapunovExponents(const JulianDay& start_time,
            const JulianDay& end_time,
            const boost::posix_time::time_duration& delta_t,
            const double min_separation,
            const double delta,
            const Field* const field) :
        Integration(start_time, end_time, delta_t, min_separation, field),
                delta_(delta), f2_(0.5 * (1 / (delta_ * delta_)))

    {
    }

    inline Triplet SetInitialPoint(const double x, const double y) const
    {
        return Triplet(x, x + delta_, x, y, y, y + delta_);
    }

    inline bool Separation(const Triplet& p) const
    {
        double d1 = Distance(p.get_x0(), p.get_y0(), p.get_x1(), p.get_y1());
        double d2 = Distance(p.get_x0(), p.get_y0(), p.get_x2(), p.get_y2());

        boost::tuple<double const&, double const&> d = boost::minmax(d1, d2);

        return d.get<1> () > min_separation_;
    }

    inline bool Compute(const Iterator& it, Triplet& p) const
    {
        double x[3], y[3];

        if (!rk_.Compute(it(), p.get_x0(), p.get_y0(), x[0], y[0])
                || !rk_.Compute(it(), p.get_x1(), p.get_y1(), x[1], y[1])
                || !rk_.Compute(it(), p.get_x2(), p.get_y2(), x[2], y[2]))
            return false;

        p = Triplet(x[0], x[1], x[2], y[0], y[1], y[2]);
        return true;
    }

    void Exponents(const Iterator& it, const Triplet& p);

    inline double get_lambda1() const
    {
        return lambda1_;
    }

    inline double get_lambda2() const
    {
        return lambda2_;
    }

    inline double get_theta1() const
    {
        return theta1_;
    }

    inline double get_theta2() const
    {
        return theta2_;
    }

};

} // namespace lagrangian

#endif /* INTEGRATION_HPP_ */
