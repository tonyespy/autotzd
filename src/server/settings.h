/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.
 *
 *  Copyright (C) 2009-2011 Nokia Corporation.
 *  Copyright (C) 2015 Canonical, Inc.
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

#ifndef AUTOTZD_SETTINGS_H
#define AUTOTZD_SETTINGS_H

#include <string>

#include <iodata/iodata.h>

#include "onitz.h"
#include "nanotime.h"
#include "tz.h"

using namespace std ;

class Autotzd ;

struct source_t
{
  virtual ~source_t() { } ;
  virtual const char* name() const = 0 ;
  virtual bool available() const = 0 ;
  virtual void load(const iodata::item *) ;
  virtual iodata::item *save() const ;
} ;

struct utc_source_t : public source_t
{
  virtual ~utc_source_t() { }
  nanotime_t value ;
  virtual nanotime_t value_at_zero() const { return nanotime_t(0,0) ; }
} ;

struct manual_utc_t : public utc_source_t
{
  manual_utc_t() { value = nanotime_t::systime_at_zero() ; }
  virtual ~manual_utc_t() { }
  const char* name() const { return "manual_utc" ; }
  bool available() const { return true ; }
  nanotime_t value_at_zero() const { return value ; }
} ;

struct nitz_utc_t : public utc_source_t
{
  nitz_utc_t() ;
  virtual ~nitz_utc_t() { }
  const char* name() const { return "nitz_utc" ; }
  bool available() const ;
  nanotime_t value_at_zero() const ;
} ;

struct gps_utc_t : public utc_source_t
{
  virtual ~gps_utc_t() { }
  const char* name() const { return "gps_utc" ; }
  bool available() const { return false ; }
} ;

struct offset_source_t : public source_t
{
  int value ;
  virtual int offset() const { return value ; }
  offset_source_t() { value = -1 ; }
  virtual ~offset_source_t() { }
  bool available() const { return value!=-1 ; }
} ;

struct manual_offset_t : public offset_source_t
{
  virtual ~manual_offset_t() { }
  const char* name() const { return "manual_offset" ; }
  void load(const iodata::item *) ;
  iodata::item *save() const ;
} ;

struct nitz_offset_t : public offset_source_t
{
  virtual ~nitz_offset_t() { }
  const char* name() const { return "nitz_offset" ; }
} ;

struct zone_source_t : public source_t
{
  string value ;
  virtual ~zone_source_t() { }
  virtual string zone() const { return value ; }
  bool available() const { return !value.empty() ; }
  void load(const iodata::item *) ;
  iodata::item *save() const ;
} ;

struct manual_zone_t : public zone_source_t
{
  virtual ~manual_zone_t() { }
  const char *name() const { return "manual_zone" ; }
} ;

struct cellular_zone_t : public zone_source_t
{
  virtual ~cellular_zone_t() { }
  suggestion_t suggestions ;
  const char *name() const { return "cellular_zone" ; }
} ;

struct key_int_t : public source_t
{
  map<string,int> m ;
  typedef map<string,int>::iterator iterator ;
  typedef map<string,int>::const_iterator const_iterator ;
  virtual ~key_int_t() { }
  int get(const string &key, int default_value=0) const ;
  bool available() const { return !m.empty(); }
  iodata::item *save() const ;
  void load(const iodata::item *) ;
} ;

struct app_snooze_t : public key_int_t
{
  virtual ~app_snooze_t() { }
  const char *name() const { return "app_snooze" ; }
};


struct source_settings : public QObject
{
  static const int min_snooze = 30 ;

  source_settings(Autotzd *owner) ;
  virtual ~source_settings() ;
  Autotzd *o ;

  zone_source_t *manual_zone ;
  cellular_zone_t *cellular_zone ;
  offset_source_t *manual_offset, *nitz_offset ;
  utc_source_t *manual_utc, *nitz_utc;

  bool time_nitz, local_cellular, auto_dst ;
  bool format_24 ;
  int offset() const ;
  string zone() const ;
  nanotime_t value_at_zero() const ;

  map<string,source_t*> src ;

  void load(const iodata::record *, const string &defult_tz) ;
  iodata::record *save() const ;

  void process_kernel_notification(const nanotime_t &jump_forwards) ;

  Q_OBJECT ;
public Q_SLOTS:
  void cellular_time_slot(const cellular_time_t &T) ;
  void cellular_zone_slot(olson *tz, suggestion_t s, bool sure) ;
} ;

#endif /* AUTOTZD_SETTINGS_H */
