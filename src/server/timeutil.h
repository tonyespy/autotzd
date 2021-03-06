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

#ifndef AUTOTZD_TIMEUTIL_H
#define AUTOTZD_TIMEUTIL_H

#include <QObject>
#include <string>
using namespace std ;

# include <iodata/iodata.h>

#include "wrappers.h"

struct switch_timezone
{
  char *old_tz ;
  switch_timezone(const char *) ;
  switch_timezone(const string &) ;
 ~switch_timezone() ;
  void constructor(const char *) ;
} ;

ticker_t now() ;
ticker_t ticker_align(ticker_t start, int step, ticker_t target) ;
ticker_t mktime_oversea(struct tm *t, const string &s) ;
ticker_t mktime_local(struct tm *t) ;
bool zone_info(int *seconds_east_of_gmt, const string &zone, string *abbreviation=NULL, ticker_t x=ticker_t()) ;
string tm_str(const struct tm *tm) ;

#endif /* AUTOTZD_TIMEUTIL_H */
