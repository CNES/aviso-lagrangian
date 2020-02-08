/*
    This file is part of lagrangian library.

    lagrangian is free software: you can redistribute it and/or modify
    it under the terms of GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lagrangian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of GNU Lesser General Public License
    along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
 */

#include "lagrangian/trace.hpp"
#include "lagrangian/datetime.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * @brief Configuration manager
 */
class Configuration {
 private:
  bool verbose_{false};

 public:
  /**
   * @brief Default constructor
   */
  Configuration() = default;

  /**
   * @brief Display a debug message
   *
   * @param message Message to display
   */
  inline void Debug(std::string const& message) {
    if (verbose_) {
      DateTime now;
      std::cerr << str(boost::format("[ %s ] %s") %
                       now.ToString("%Y-%m-%d %H:%M:%S.%f") % message)
                << std::endl;
      std::cerr.flush();
    }
  }

  /**
   * @brief Enable or disable verbose mode
   *
   * @param value Value to enable (true) or disable (false) verbose mode
   */
  inline void set_verbose(const bool value) { verbose_ = value; }

  /**
   * @brief Get the version number
   *
   * @return The version number
   */
  static auto Version() -> std::string { return "2.3.0"; }
};

static Configuration g_configuration;

void SetVerbose(const bool value) { g_configuration.set_verbose(value); }

void Debug(std::string const& message) { g_configuration.Debug(message); }

auto Version() -> std::string { return lagrangian::Configuration::Version(); }

}  // namespace lagrangian
