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

#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>

#include "nanotime.h"
#include "../common/log.h"

nanotime_t nanotime_t::div2() const
{
  uint32_t y = nano() / 2 ;
  time_t x = sec() ;
  if (x%2 != 0)
    x--, y+=NANO/2 ;
  x /= 2 ;
  while (y>=NANO)
    ++x, y-=NANO ;
  return nanotime_t(x,y) ;
}

nanotime_t nanotime_t::systime_at_zero() // TODO: rename it to start_up_time()
{
  nanotime_t s = systime_now() ;
  nanotime_t m = monotonic_now() ;
  if(s.is_invalid() || m.is_invalid())
    return nanotime_t() ; // invalid
  return s-m ;
}

nanotime_t nanotime_t::systime_now()
{
  struct timeval tv ;
  int res = gettimeofday(&tv, NULL) ;
  if(res<0)
    return nanotime_t() ; // invalid
  nanotime_t t(tv.tv_sec, tv.tv_usec*1000) ; // micro to nano
  t.fix_overflow() ;
  return t ;
}

nanotime_t nanotime_t::monotonic_now()
{
  struct timespec ts;
  memset(&ts, 0, sizeof(struct timespec));
  int res = -1;
#if defined(CLOCK_BOOTTIME)
  res = clock_gettime(CLOCK_BOOTTIME, &ts);
#endif
  if (res < 0)
    res = clock_gettime(CLOCK_MONOTONIC, &ts);

  nanotime_t t = nanotime_t::from_timespec(ts);

  if(res < 0) {
    log_error("clock_gettime error: %m");
    t.invalidate() ;
  } else {
    t.fix_overflow() ;
  }
  return t ;
}

int nanotime_t::set_systime(const nanotime_t &t)
{
  if(t.is_invalid() || !t.is_normalized())
  {
    errno = EINVAL ;
    return -1 ;
  }
  int micro = t.nano() / 1000 ;
  bool next_micro = 500 <= t.nano() - 1000*micro ;
  struct timeval tv = { t.sec(), micro } ;
  if(next_micro && ++tv.tv_usec==NANO/1000)
    ++tv.tv_sec, tv.tv_usec = 0 ;
  log_info("executing settimeofday(sec=%lld, usec=%ld", (long long)tv.tv_sec, tv.tv_usec) ;
  int res = settimeofday(&tv, NULL) ;
  log_info("settimeofday() returned '%d'", res) ;
  return res ;
}

int nanotime_t::set_systime_at_zero(const nanotime_t &t)
{
  if(t.is_invalid() || !t.is_normalized())
  {
    errno = EINVAL ;
    return -1 ;
  }
  nanotime_t m=monotonic_now() ;
  if(m.is_invalid())
    return -1 ;
  return set_systime(t+m) ;
}
