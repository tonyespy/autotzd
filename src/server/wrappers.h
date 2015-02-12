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

#ifndef AUTOTZD_WRAPPERS_H
#define AUTOTZD_WRAPPERS_H

struct ticker_t
{
private:
  time_t t ;
public:
  ticker_t() : t(0) { }
  explicit ticker_t(time_t v) : t(v) { }
  time_t value() const { return t ; }
  bool is_valid() const { return t>0 ; }
  bool operator<(const ticker_t &y) const { return value() < y.value() ; }
  bool operator<=(const ticker_t &y) const { return value() <= y.value() ; }
  int operator-(const ticker_t &y) const { return value()-y.value() ; }
  ticker_t operator+(int diff) const { return ticker_t(value()+diff) ; }
  void operator-=(time_t diff) { t-=diff ; }
} ;

struct cookie_t
{
private:
  unsigned val ;
public:
  cookie_t() : val(0) { }
  explicit cookie_t(unsigned v) : val(v) { }
  unsigned value() const { return val ; }
  bool is_valid() const { return val>0 ; }
  const cookie_t & operator++() { ++val ; return *this ; }
  bool operator<(const cookie_t &y) const { return val < y.val ; }
} ;

#endif /* AUTOTZD_WRAPPERS_H */
