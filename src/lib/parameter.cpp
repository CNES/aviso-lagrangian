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
    along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <regex>

// ___________________________________________________________________________//

#include "lagrangian/parameter.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

// Interpolation of an environment variable. If the variable is not found, the
// part of the chain containing the variable is not changed.
static std::string ExpandShell(std::string& s) {
  static const std::regex re(R"(\$\{(\w+)\})");

  std::match_results<std::string::const_iterator> what;
  std::string::const_iterator start = s.begin();
  std::string::const_iterator end = s.end();

  while (regex_search(start, end, what, re)) {
    char* env = getenv(what.str(1).c_str());

    if (env) {
      boost::replace_first(s, what.str(0), env);
      start = s.begin();
      end = s.end();
    } else {
      start = what[1].second;
    }
  }
  return s;
}

/*
 * @brief Analysis of the line loaded into memory.
 *
 * @param line Line analysis
 * @param buffer Buffer to update if necessary
 *
 * @return True if parsing is OK if not false
 */
bool Parameter::Parse(std::string& line, std::string& buffer) {
  boost::trim(line);

  // Delete comment
  auto it = boost::find_first(line, "#");
  if (!it.empty()) {
    boost::erase_range(line,
                       boost::make_iterator_range(it.begin(), line.end()));
  }

  buffer += line;

  // search for line continuation
  it = boost::find_first(buffer, "\\");
  if (!it.empty()) {
    boost::erase_range(buffer,
                       boost::make_iterator_range(it.begin(), buffer.end()));
  } else {
    if (buffer.length()) {
      static const std::regex re("^([^=]*)=(.*)");
      std::smatch what;

      if (std::regex_match(buffer, what, re)) {
        std::string key = what[1].str();
        std::string val = what[2].str();

        boost::trim(key);
        boost::trim(val);

        data_[key].push_back(ExpandShell(val));
      } else {
        return false;
      }
    }
    buffer = "";
  }
  return true;
}

/*
 * @brief Loads the configuration file.
 *
 * @param filename File to load into memory
 *
 * @throw std::runtime_error if the file does not exist or contains a syntax
 * error
 */
void Parameter::Load(const std::string& filename) {
  std::ifstream infile;
  std::string buffer = "";
  int line_number = 0;

  infile.open(filename.c_str(), std::ifstream::in);
  if (!infile.is_open())
    throw std::runtime_error(
        boost::str(boost::format("Couldn't open `%s' for reading") % filename));

  while (!infile.eof()) {
    static const std::regex re(R"(^\s*#include\s+"(.*)\")");
    std::string line;
    std::smatch what;

    ++line_number;

    getline(infile, line);
    if (regex_match(line, what, re)) Load(what[1].str());

    if (!Parse(line, buffer))
      throw std::runtime_error(boost::str(
          boost::format("syntax error line %d: %s") % line_number % line));
  }
}
}  // namespace lagrangian
