/***************************************************************************
**                                                                        **
**   Copyright (C) 2009-2011 Nokia Corporation.                           **
**                                                                        **
**   Author: Ilya Dogolazky <ilya.dogolazky@nokia.com>                    **
**   Author: Simo Piiroinen <simo.piiroinen@nokia.com>                    **
**   Author: Victor Portnov <ext-victor.portnov@nokia.com>                **
**                                                                        **
**     This file is part of Timed                                         **
**                                                                        **
**     Timed is free software; you can redistribute it and/or modify      **
**     it under the terms of the GNU Lesser General Public License        **
**     version 2.1 as published by the Free Software Foundation.          **
**                                                                        **
**     Timed is distributed in the hope that it will be useful, but       **
**     WITHOUT ANY WARRANTY;  without even the implied warranty  of       **
**     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               **
**     See the GNU Lesser General Public License  for more details.       **
**                                                                        **
**   You should have received a copy of the GNU  Lesser General Public    **
**   License along with Timed. If not, see http://www.gnu.org/licenses/   **
**                                                                        **
***************************************************************************/
#include <errno.h>
#include <string>
#include <stdlib.h>
using namespace std ;

#include "../common/log.h"

#include "timeutil.h"
#include "wrappers.h"

ticker_t now()
{
  return ticker_t(time(NULL)) ;
}

ticker_t ticker_align(ticker_t start, int step, ticker_t target)
{
  // log_debug("start=%ld, step=%d, target=%ld", start.value(), step, target.value()) ;
  log_assert(step>0) ;
  log_assert(start.is_valid()) ;
  log_assert(target.is_valid()) ;

  ticker_t result ;

  if(start<=target)
  {
    int delta = target - start ;
    int x = delta/step + 1 ;
    result = start + x*step ;
    // log_debug("delta=%d, x=%d, result=%ld", delta, x, result.value()) ;
  }
  else
  {
    int delta = start - target ;
    int x = delta/step - (delta%step==0) ;
    result = start + (-x)*step ;
    // log_debug("delta=%d, x=%d, result=%ld", delta, x, result.value()) ;
  }
  log_assert((start-result) % step == 0) ;
  log_assert(target<result) ;
  log_assert(result-target<=step) ; // 1 <= result-target <= step

  return result ;
}

void switch_timezone::constructor(const char *new_tz)
{
  bool new_is_empty = new_tz==NULL || *new_tz=='\0' ;

  const char *env = getenv("TZ") ;
  if(env==NULL)
    old_tz = NULL ;
  else
  {
    old_tz = strdup(env) ;
    log_assert(old_tz!=NULL) ;
  }

  int res = new_is_empty ? unsetenv("TZ") : setenv("TZ", new_tz, true) ;

  if(res<0)
    log_critical("Can't change TZ environment: %s", strerror(errno)) ;

  tzset() ;
}

switch_timezone::switch_timezone(const char *value)
{
  constructor(value) ;
}

switch_timezone::switch_timezone(const string &s)
{
  constructor(s.c_str()) ;
}

switch_timezone::~switch_timezone()
{
  int res = old_tz ? setenv("TZ", old_tz, true) : unsetenv("TZ") ;

  if(res<0)
    log_critical("Can't change TZ environment: %s", strerror(errno)) ;

  if(old_tz)
    free(old_tz) ;

  tzset() ;
}

ticker_t mktime_oversea(struct tm *t, const string &s)
{
  switch_timezone tmp(s) ;
  return ticker_t(mktime(t)) ;
}

ticker_t mktime_local(struct tm *t)
{
  return ticker_t(mktime(t)) ;
}

bool zone_info(int *seconds_east_of_gmt, const string &zone, string *abbreviation, ticker_t x)
{
  string tz ;
  if(!zone.empty())
    tz = (string) ":" + zone ;
  switch_timezone tmp(tz) ;
  time_t t = x.is_valid() ? x.value() : time(NULL) ;
  struct tm tm ;
  if(localtime_r(&t, &tm)==0)
    return false ;
  if(seconds_east_of_gmt)
    *seconds_east_of_gmt = tm.tm_gmtoff ;
  if(abbreviation)
    *abbreviation = tm.tm_zone ;
  // log_debug("tm_zone='%s'", tm.tm_zone) ;
  return true ;
}

string tm_str(const struct tm *tm)
{
  ostringstream os ;
#define X(x) #x"=" << tm->tm_##x
#define Y(x) X(x) << ", "
  os << "{" << Y(year) Y(mon) Y(mday) Y(hour) Y(min) Y(sec) Y(wday) Y(yday) X(isdst) << "}" ;
#undef X
#undef Y
  return os.str() ;
}
