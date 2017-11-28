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

#include <boost/lexical_cast.hpp>
#include <iostream>

// ___________________________________________________________________________//

#include "lagrangian/units.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

static void HandleParseStatus(ut_unit* unit, const std::string& str) {
  if (unit == nullptr) {
    ut_status status = ut_get_status();
    if (status == UT_BAD_ARG)
      throw units::Exception("empty units attribute string");
    if (status == UT_SYNTAX)
      throw units::Exception("'" + str + "' contained a syntax error");
    if (status == UT_UNKNOWN)
      throw units::Exception("'" + str +
                             "' string contained an unknown identifier");
    throw units::Exception("Unhandled exception: " +
                           boost::lexical_cast<std::string>(status));
  }
}

static void HandleConverterStatus(cv_converter* converter,
                                  const std::string& from,
                                  const std::string& to) {
  if (converter == nullptr) {
    ut_status status = ut_get_status();
    if (status == UT_BAD_ARG) throw units::Exception("one of units is null");
    if (status == UT_NOT_SAME_SYSTEM)
      throw units::Exception("the units '" + from + "' and '" + to +
                             "' don't belong to the same unit-system");
    if (status == UT_MEANINGLESS)
      throw units::Exception(
          "the units '" + from + "' and '" + to +
          "' belong to the same unit-system but conversion " +
          "between them is meaningless");
    throw units::Exception("Unhandled exception: " +
                           boost::lexical_cast<std::string>(status));
  }
}

// ___________________________________________________________________________//

static units::SmartUtSystem g_system;

// ___________________________________________________________________________//

UnitConverter Units::GetConverter(const std::string& from,
                                  const std::string& to) {
  if (from == to) {
    return {};
  } else {
    g_system.Allocates();

    ut_unit* ut_from = ut_parse(g_system.get(), from.c_str(), UT_UTF8);
    HandleParseStatus(ut_from, from);

    ut_unit* ut_to = ut_parse(g_system.get(), to.c_str(), UT_UTF8);
    HandleParseStatus(ut_from, to);

    cv_converter* converter = ut_get_converter(ut_from, ut_to);
    HandleConverterStatus(converter, from, to);

    double offset = cv_convert_double(converter, 0);
    double scale = cv_convert_double(converter, 1) - offset;

    ut_free(ut_from);
    ut_free(ut_to);
    cv_free(converter);

    return {offset, scale};
  }
}

bool Units::AreConvertible(const std::string& unit1, const std::string& unit2) {
  g_system.Allocates();

  ut_unit* ut_unit1 = ut_parse(g_system.get(), unit1.c_str(), UT_UTF8);
  HandleParseStatus(ut_unit1, unit1);

  ut_unit* ut_unit2 = ut_parse(g_system.get(), unit2.c_str(), UT_UTF8);
  HandleParseStatus(ut_unit2, unit2);

  int result = ut_are_convertible(ut_unit1, ut_unit2);

  ut_free(ut_unit1);
  ut_free(ut_unit2);

  return result != 0;
}
}  // namespace lagrangian
