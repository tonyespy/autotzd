/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.
 *
 *  Copyright (C) 2009-2011 Nokia Corporation.
 *  Copyright (C) 2015 Canonical Ltd.
 *
 *  autotzd is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.
 *
 *  autotzd is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY;  without even the implied warranty  of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU Lesser General Public License  for more details.
 *
 *  You should have received a copy of the GNU  Lesser General Public
 *  License along with autotzd. If not, see http://www.gnu.org/licenses/
 *
 */

#ifndef AUTOTZD_TZDATA_H
#define AUTOTZD_TZDATA_H

#include <string>
#include <set>

class olson ;

namespace tzdata
{
  enum zone_type { All_Zones, Main_Zones, Real_Zones } ;

  std::string iso_3166_1_alpha2_by_mcc(const std::string &mcc) ;
  int by_country(const std::string &alpha2, enum zone_type type, std::set<olson*> &out) ;
  olson* country_default(const std::string &alpha2) ;
  olson* device_default() ;
  int filter(const std::set<olson*> &in, time_t moment, int offset, int dst, std::set<olson*> &out) ;
  bool is_single_zone_country(const std::string &alpha2) ;

  std::string set_str(const std::set<olson*> &) ;
  void init(const std::string &default_tz) ;
}

#endif /* AUTOTZD_TZDATA_H */
