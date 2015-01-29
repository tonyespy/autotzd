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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QMetaMethod>

#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>

#include "../lib/wall-settings.h"
#include "../lib/aliases.h"

#include "flags.h"
#include "timed.h"
#include "settings.h"
#include "misc.h"
#include "onitz.h"

#define LOCALTIMELINK "/var/lib/timed/localtime"

void source_t::load(const iodata::item *)
{
  // empty, do nothing
}

iodata::item *source_t::save() const
{
  return new iodata::record ;
}

nitz_utc_t::nitz_utc_t()
{
  value.invalidate() ;
}

bool nitz_utc_t::available() const
{
  return ! value.is_invalid() ;
}

nanotime_t nitz_utc_t::value_at_zero() const
{
  return value ;
}

void manual_offset_t::load(const iodata::item *r)
{
  value = r->get("value")->value() ;
}

iodata::item *manual_offset_t::save() const
{
  iodata::record *r = new iodata::record ;
  r->add("value", value) ;
  return r ;
}

void zone_source_t::load(const iodata::item *r)
{
  value = r->get("value")->str() ;
}

iodata::item *zone_source_t::save() const
{
  iodata::record *r = new iodata::record ;
  r->add("value", value) ;
  return r ;
}

int key_int_t::get(const string &key, int default_value) const
{
  const_iterator it=m.find(key) ;
  return it==m.end() ? default_value : it->second ;
}

iodata::item *key_int_t::save() const
{
    iodata::array *a = new iodata::array ;
    for(const_iterator it=m.begin(); it!=m.end(); ++it)
    {
      iodata::record *r = new iodata::record ;
      r->add("key", new iodata::bytes(it->first)) ;
      r->add("value", it->second) ;
      a->add(r) ;
    }
    return a ;
}

void key_int_t::load(const iodata::item *a)
{
    m.clear();
    for(unsigned i=0; i<a->size(); ++i)
    {
      const iodata::item *r = a->get(i) ;
      m[r->get("key")->str()]=r->get("value")->value() ;
    }
}

source_settings::source_settings(Timed *owner) : QObject(owner)
{
  log_debug() ;
  o = owner ;
  time_nitz = true ;
  local_cellular = true ;
  auto_dst = true ;
  format_24 = true ;

#define _creat(xxx) src[#xxx] = xxx = new xxx##_t ;
  _creat(manual_utc) ;
  _creat(nitz_utc) ;
  _creat(manual_offset) ;
  _creat(nitz_offset) ;
  _creat(manual_zone) ;
  _creat(cellular_zone) ;
#undef _creat // spell it without 'e' ;-)
}

source_settings::~source_settings()
{
  for (map<string, source_t*>::iterator it=src.begin(); it!=src.end(); ++it)
    delete it->second ;
}

void source_settings::load(const iodata::record *r, const string &default_tz)
{
  log_debug() ;
  log_debug("CUST time_nitz: %d", r->get("time_nitz")->value());
  log_debug("CUST format_24: %d", r->get("format_24")->value());
  time_nitz = r->get("time_nitz")->value() != 0 ;
  local_cellular = r->get("local_cellular")->value() != 0 ;
  auto_dst = r->get("auto_dst")->value() != 0 ;
  format_24 = r->get("format_24")->value() != 0 ;

  for(map<string,source_t*>::iterator it=src.begin(); it!=src.end(); ++it)
  {
    log_debug("it: '%s'", it->first.c_str()) ;
    it->second->load(r->get(it->first)) ;
    if (zone_source_t *z = dynamic_cast<zone_source_t*> (it->second))
    {
      if (z->value == "[unknown]")
        z->value = default_tz ;
    }
    log_debug() ;
  }
}

iodata::record *source_settings::save() const
{
  iodata::record *r = new iodata::record ;
  r->add("time_nitz",time_nitz) ;
  r->add("local_cellular", local_cellular) ;
  r->add("auto_dst", auto_dst) ;
  r->add("format_24", format_24) ;
  for(map<string,source_t*>::const_iterator it=src.begin(); it!=src.end(); ++it)
    r->add(it->first, it->second->save()) ;
  return r ;
}

int source_settings::offset() const
{
  if(local_cellular && nitz_offset->available())
    return nitz_offset->offset() ;
  else
    return manual_offset->offset() ;
}

string source_settings::zone() const
{
  if(local_cellular && cellular_zone->available())
    return cellular_zone->zone() ;
  else
    return manual_zone->zone() ;
}

nanotime_t source_settings::value_at_zero() const
{
  if(time_nitz && nitz_utc->available())
    return nitz_utc->value_at_zero() ;
  else
    return manual_utc->value_at_zero() ;
}

void source_settings::process_kernel_notification(const nanotime_t &jump_forwards)
{
  o->invoke_signal(-jump_forwards) ;

  // Make time valid
  //  o->open_epoch() ;
}

void source_settings::cellular_time_slot(const cellular_time_t &T)
{
  nanotime_t time_at_zero = nanotime_t(T.value) - T.ts ;
  log_debug("time_at_zero=%s, (T=%s)", time_at_zero.str().c_str(), T.str().c_str()) ;
  nitz_utc->value = time_at_zero ;
  if(time_nitz) // we want to use nitz as time source
  {
    //    set_system_time(nitz_utc->value_at_zero()) ;
    //    o->open_epoch() ;
  }
}

void source_settings::cellular_zone_slot(olson *tz, suggestion_t s, bool sure)
{
  (void) sure ;
  log_debug("time zone '%s' magicaly detected", tz->name().c_str()) ;
  cellular_zone->value = tz->name() ;
  cellular_zone->suggestions = s ;
  if(local_cellular)
  {
    //    fix_etc_localtime() ;
    // TODO: update_oracle_context(true) ;
  }
  o->invoke_signal() ;
}
