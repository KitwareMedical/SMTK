//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_attribute_DateTime_h
#define __smtk_attribute_DateTime_h

#include "smtk/CoreExports.h"
#include "smtk/SystemConfig.h"
#include "smtk/common/CompilerInformation.h"

#ifndef SHIBOKEN_SKIP
SMTK_THIRDPARTY_PRE_INCLUDE
#include <boost/date_time/posix_time/ptime.hpp>
SMTK_THIRDPARTY_POST_INCLUDE
#endif

#include <string>

namespace smtk {
  namespace attribute {

class TimeZone;

//.NAME DateTime - Date & time representation generally based on ISO 8601.
//.SECTION Description
// A minimal wrapper for boost::posix_time::ptime
class SMTKCORE_EXPORT DateTime
{
public:
  DateTime();

  /// Explicitly sets each component, with optional time zone conversion
  bool setComponents(
    int year,
    int month = 1,
    int day = 1,
    int hour = 0,
    int minute = 0,
    int second = 0,
    int millisecond = 0,
    TimeZone *timeZone = NULL);

  // Returns each component, with optional time zone conversion
  bool components(
    int& year,
    int& month,
    int& day,
    int& hour,
    int& minute,
    int& second,
    int& millisecond,
    TimeZone *timeZone = NULL) const;

  /// Indicates if instance represents valid datetime value
  bool isSet() const;

  /// Parses datetime string in "strict" format: YYYYMMDDThhmmss[.uuuuuu]
  bool parse(const std::string& ts);

  /// Parses using boost time_from_string(), which is NOT ISO COMPLIANT
  bool parseBoostFormat(const std::string& ts);

  // Todo bool parse(formatString, dataString);
  // General parser using boost datetime I/O classes

  // Relational operators
  bool operator==(const DateTime& dt) const;
  bool operator<(const DateTime& dt) const;
  bool operator>(const DateTime& dt) const;

protected:
  boost::posix_time::ptime m_ptime;
};

  } // namespace attribute
} // namespace smtk

#endif // __smtk_attribute_DateTime_h
