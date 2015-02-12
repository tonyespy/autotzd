/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.
 *
 *  Copyright (C) 2009-2011 Nokia Corporation.
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

#ifndef AUTOTZD_OLSON_H
#define AUTOTZD_OLSON_H

#include <map>
#include <string>

class olson
{
  std::string zone_name ;
  static std::map<std::string, olson*> *zonetab ;

  olson(const std::string &name) ;

public:
  std::string name() { return zone_name ; }
  static olson *by_name(const std::string &) ;
  static olson *by_offset(int offset) ;
  static void destructor() ;
  bool match(time_t at, int offset, int dst_flag) ;
} ;

#endif /* AUTOTZD_OLSON_H */
