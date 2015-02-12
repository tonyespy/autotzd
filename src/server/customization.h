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

#ifndef AUTOTZD_CUSTOMIZATION_H
#define AUTOTZD_CUSTOMIZATION_H

#include <iodata/storage.h>
#include <iodata/iodata.h>

struct customization_t
{
  bool format24() { return format_24_value ; }
  bool useNitz() { return use_nitz_value ; }
  bool autoTime() { return auto_time_value ; }
  bool guessTz() { return guess_tz_value ; }

  customization_t() ;
  iodata::storage *file ;
  iodata::record *rec ;
} ;

#endif /* AUTOTZD_CUSTOMIZATION_H */
