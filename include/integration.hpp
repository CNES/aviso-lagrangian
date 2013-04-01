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

#include <boost/date_time.hpp>
#include <boost/math/special_functions.hpp>
#include <limits>

// ___________________________________________________________________________//

#include "field.hpp"
#include "julian_day.hpp"
#include "runge_kutta.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

/**
 * @brief Definition of an iterator over a time period
 */
class Iterator
{
private:
    double end_;
    double inc_;
    double ix_;

public:

    /**
     * @brief Default constructor
     *
     * @param begin Begin of the period expressed in number of seconds
     * elapsed since 1970
     * @param end End of the period expressed in number of seconds
     * elapsed since 1970
     * @param inc Time increment in seconds
     */
    Iterator(const double begin, const double end, const double inc) :
        end_(end), inc_(begin > end ? -inc : inc), ix_(begin)
    {
    }

    /**
     * @brief Test whether there is still a time step to cross into the time
     * interval defined.
     *
     * @return True if the path in the time interval is not complete.
     */
    inline bool GoAfter() const
    {
        return inc_ > 0 ? ix_ <= end_ : ix_ >= end_;
    }

    /**
     * @brief Move to the next time step.
     */
    inline void operator++()
    {
        ix_ += inc_;
    }

    /**
     * @brief Get the current time in the time interval defined
     *
     * @return The current time expressed in number of seconds elapsed since
     * 1970
     */
    inline double operator()() const
    {
        return ix_;
    }

    /**
     * @brief Get the time step defined
     *
     * @return The time step expressed in seconds
     */
    inline double inc() const
    {
        return inc_;
    }
};

// ___________________________________________________________________________//

class Integration
{
protected:
    double size_of_interval_;
    Field* field_;
    double start_time_;
    double end_time_;
    RungeKutta rk_;

public:

    /**
     * @brief Default constructor
     *
     * @param start_time Start time of the integration (number of seconds
     * elapsed 1970)
     * @param end_time End date of the integration (number of seconds
     * elapsed 1970)
     * @param delta_t Time interval, in seconds
     * @param field Field to use for computing the velocity of a point.
     *
     * @throw std::runtime_error if the time interval is negative
     */
    Integration(const JulianDay& start_time,
            const JulianDay& end_time,
            const boost::posix_time::time_duration& delta_t,
            Field* field) :
        size_of_interval_(delta_t.total_seconds()), field_(field),
                start_time_(start_time.ToUnixTime()),
                end_time_(end_time.ToUnixTime()),
                rk_(size_of_interval_ * (start_time_ > end_time_ ? -1: 1),
                        field_)
    {
        if (size_of_interval_ < 0)
            throw std::runtime_error("Time delta must be positive");
    }

    /**
     * @brief Default method invoked when an instance is destroyed.
     */
    virtual ~Integration()
    {
    }

    /**
     * @brief Return an iterator that describes the integration period
     *
     * @return The iterator
     */
    Iterator GetIterator() const
    {
        return Iterator(start_time_, end_time_, size_of_interval_);
    }

    /**
     * Perform the tasks before a new time step (eg load grids required)
     *
     * @param t Time step in seconds
     */
    void Fetch(const double t) const
    {
        start_time_ < end_time_
            ? field_->Fetch(t, t + size_of_interval_)
            : field_->Fetch(t, t - size_of_interval_);
    }
};

// ___________________________________________________________________________//

class Path: public Integration
{
public:

    /**
     * @brief Default constructor
     *
     * @param start_time Start time of the integration (number of seconds
     * elapsed 1970)
     * @param end_time End date of the integration (number of seconds
     * elapsed 1970)
     * @param delta_t Time interval, in seconds
     * @param field Field to use for computing the velocity of a point.
     */
     Path(const JulianDay& start_time,
            const JulianDay& end_time,
            const boost::posix_time::time_duration& delta_t,
            Field* field) :
                Integration(start_time,
                        end_time,
                        delta_t,
                        field)
    {
    }

    /**
     * @brief Default method invoked when an instance is destroyed.
     */
    virtual ~Path()
    {
    }

    /**
     * @brief Calculate the new position of the particle
     *
     * @param it Iterator
     * @param x0 Longitude in degrees
     * @param y0 Latitude in degrees
     * @param x1 New longitude in degrees
     * @param y1 New latitude in degrees
     *
     * @return true if the new position is defined otherwise false.
     */
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

/**
 * @brief Define the position of three points
 */
class Triplet
{
private:
    double x0_, x1_, x2_;
    double y0_, y1_, y2_;
    double time_;
    bool completed_;

public:

    /**
     * @brief Default constructor
     */
    Triplet() :
            x0_(), x1_(), x2_(), y0_(), y1_(), y2_(), time_(), completed_()
    {
    }

    /**
     * @brief Construct a new object defining the position of the 3 points
     *
     * @param x0 Longitude of point #1
     * @param x1 Longitude of point #2
     * @param x2 Longitude of point #3
     * @param y0 Latitude of point #1
     * @param y1 Latitude of point #2
     * @param y2 Latitude of point #3
     */
    Triplet(const double x0,
            const double x1,
            const double x2,
            const double y0,
            const double y1,
            const double y2) :
            x0_(x0), x1_(x1), x2_(x2), y0_(y0), y1_(y1), y2_(y2), time_(0), completed_(false)
    {
    }

    /**
     * @brief Update the position
     *
     * @param time Time step (number of seconds elapsed since 1970)
     * @param x0 Longitude of point #1
     * @param x1 Longitude of point #2
     * @param x2 Longitude of point #3
     * @param y0 Latitude of point #1
     * @param y1 Latitude of point #2
     * @param y2 Latitude of point #3
     */
    inline void Update(const double time,
            const double x0,
            const double x1,
            const double x2,
            const double y0,
            const double y1,
            const double y2)
    {
        x0_ = x0;
        x1_ = x1;
        x2_ = x2;
        y0_ = y0;
        y1_ = y1;
        y2_ = y2;
        time_ = time;
    }

    /**
     * @brief Get the longitude of the point #1
     *
     * @return The longitude in degrees
     */
    inline double get_x0() const
    {
        return x0_;
    }

    /**
     * @brief Get the longitude of the point #2
     *
     * @return The longitude in degrees
     */
    inline double get_x1() const
    {
        return x1_;
    }

    /**
     * @brief Get the longitude of the point #3
     *
     * @return The longitude in degrees
     */
    inline double get_x2() const
    {
        return x2_;
    }

    /**
     * @brief Get the latitude of the point #1
     *
     * @return The latitude in degrees
     */
    inline double get_y0() const
    {
        return y0_;
    }

    /**
     * @brief Get the latitude of the point #2
     *
     * @return The latitude in degrees
     */
    inline double get_y1() const
    {
        return y1_;
    }

    /**
     * @brief Get the latitude of the point #3
     *
     * @return The latitude in degrees
     */
    inline double get_y2() const
    {
        return y2_;
    }

    /**
     * @brief Get the time at the end of the integration
     *
     * @return The time expressed in number of seconds elapsed since 1970
     */
    inline double get_time() const
    {
        return time_;
    }

    /**
     * @brief Test if the integration is over
     *
     * @return True if the integration is over
     */
    inline bool get_completed() const
    {
        return completed_;
    }

    /**
     * @brief Indicate that the integration is complete.
     */
    inline void set_completed()
    {
        completed_ = true;
    }

    /**
     * @brief Test if the integration is defined.
     *
     * @return True if the integration is defined.
     */
    inline bool IsMissing()
    {
        return std::isnan(x0_) || std::isnan(x1_) || std::isnan(x2_)
                || std::isnan(y0_) || std::isnan(y1_) || std::isnan(y2_);
    }

    /**
     * @brief Return the location of a missing dot
     *
     * @return The missing dot
     */
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
public:
    /**
     * @brief Mode of integration
     */
    enum Mode
    {
        kFSLE, kFTLE
    };

private:
    typedef bool
    (FiniteLyapunovExponents::*SeparationFunction)(const Triplet& p) const;

    const double delta_;
    double min_separation_;
    Mode mode_;
    SeparationFunction pSeparation_;
    double f2_;
    double lambda1_;
    double lambda2_;
    double theta1_;
    double theta2_;

    inline bool SeparationFSLE(const Triplet& p) const
    {
        double d1 = Distance(p.get_x0(), p.get_y0(), p.get_x1(), p.get_y1());
        double d2 = Distance(p.get_x0(), p.get_y0(), p.get_x2(), p.get_y2());

        return (d1 > d2 ? d1: d2) > min_separation_;
    }

    inline bool SeparationFTLE(const Triplet&) const
    {
        return false;
    }

public:

    /**
     * @brief Default constructor
     *
     * @param start_time Start time of the integration
     * @param end_time End date of the integration
     * @param delta_t Time interval
     * @param mode Integration mode
     * @param min_separation Minimal separation in degrees
     * @param delta The gap between two consecutive dots, in degrees, of the grid
     * @param field Field to use for computing the velocity of a point.
     *
     * @throw std::invalid_argument if the mode of integration is unknown.
     */
    FiniteLyapunovExponents(const JulianDay& start_time,
            const JulianDay& end_time,
            const boost::posix_time::time_duration& delta_t,
            const Mode mode,
            const double min_separation,
            const double delta,
            Field* field) :
        Integration(start_time, end_time, delta_t, field),
                delta_(delta), mode_(mode), f2_(0.5 * (1 / (delta_ * delta_))),
                lambda1_(), lambda2_(), theta1_(), theta2_()
    {
        switch (mode_)
        {
        case kFSLE:
            min_separation_ = min_separation;
            pSeparation_ = &FiniteLyapunovExponents::SeparationFSLE;
            break;
        case kFTLE:
            min_separation_ = -1;
            pSeparation_ = &FiniteLyapunovExponents::SeparationFTLE;
            break;
        default:
            throw std::invalid_argument(
                    "invalid FiniteLyapunovExponents::Mode computation");
        }
    }

    /**
     * @brief Default method invoked when an instance is destroyed.
     */
    virtual ~FiniteLyapunovExponents()
    {
    }

    /**
     * @brief Set the value of the initial point
     * @param x Longitude
     * @param y Latitude
     *
     * @return The position of the initial point
     */
    inline Triplet SetInitialPoint(const double x, const double y) const
    {
        return Triplet(x, x + delta_, x, y, y, y + delta_);
    }

    /**
     * @brief Determine whether the particle is deemed to be separate
     *
     * @param p Position of the particle
     * @return True if the particle is separated.
     */
    inline bool Separation(const Triplet& p) const
    {
        return (this->*pSeparation_)(p);
    }

    /**
     * @brief Get mode of integration
     *
     * @return The mode of the integration
     */
    inline Mode get_mode() const
    {
        return mode_;
    }

    /**
     * @brief Calculate the integration
     *
     * @param it Iterator
     * @param p Position of the particle
     * @param cell Cell properties of the grid used for the interpolation
     * @return True the integration is defined otherwise false
     */
    inline bool Compute(const Iterator& it, Triplet& p,
            CellProperties& cell) const
    {
        double x[3], y[3];

        if (!rk_.Compute(it(), p.get_x0(), p.get_y0(), x[0], y[0], cell)
                || !rk_.Compute(it(), p.get_x1(), p.get_y1(), x[1], y[1], cell)
                || !rk_.Compute(it(), p.get_x2(), p.get_y2(), x[2], y[2], cell))
            return false;

        p.Update(it(), x[0], x[1], x[2], y[0], y[1], y[2]);
        return true;
    }

    /**
     * @brief Calculate the exponent
     *
     * @param p Position of the particle
     * @return True if the exponents are defined
     */
    bool Exponents(const Triplet& p);

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
