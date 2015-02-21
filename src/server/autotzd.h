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

#ifndef AUTOTZD_H
#define AUTOTZD_H

#include <QCoreApplication>
#include <QMetaMethod>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>

#include <iodata/validator.h>
#include <iodata/storage.h>

#include "wrappers.h"
#include "unix-signal.h"
#include "onitz.h"
#include "olson.h"
#include "tz.h"
#include "csd.h"
#include "timedatewatcher.h"

struct Autotzd : public QCoreApplication
{
public:
  inline const char *configuration_path() { return  "/etc/autotzd.rc" ; }

private:

  bool format24_by_default ;
  bool auto_time_by_default ;
  bool guess_tz_by_default ;

  bool nitz_supported ;
  string tz_by_default ;
  bool first_boot_date_adjusted;
  TimedateWatcher *td_watcher;

public:
  bool is_nitz_supported() { return nitz_supported ; }
  const string &default_timezone() { return tz_by_default ; }
  const QString get_settings_path() { return settings_path; }
  void init_first_boot_hwclock_time_adjustment_check();

private:

  // init_* methods, to be called by constructor only
  void init_unix_signal_handler() ;
  void init_configuration() ;
  void init_default_properties() ;
  //  void init_main_interface_object() ;
  //  void init_main_interface_dbus_name() ;
  void init_timedate_service() ;
  void init_cellular_services() ;

public:
  void stop_stuff() ;
  void stop_dbus() ;
  cellular_handler *nitz_object ;
  csd_t *csd ;

  QDBusConnectionInterface *ses_iface ;

  map<int,unsigned> children ;

public:
  Autotzd(int ac, char **av) ;
  virtual ~Autotzd() ;
  int get_default_gmt_offset() { return default_gmt_offset ; }

private:

  unsigned threshold_period_long, threshold_period_short ;
  QString data_path, events_path, settings_path;
  int default_gmt_offset ;
  void load_rc() ;
  void load_settings() ;
public:
  void save_settings() ;
private:
  bool signal_invoked ;
  nanotime_t systime_back ;
  tz_oracle_t *tz_oracle ;

public:
  void invoke_signal(const nanotime_t &) ;
  void invoke_signal() { nanotime_t zero=0 ; invoke_signal(zero) ; }

signals:
    void new_timezone_slot(const std::string &timezone);

public Q_SLOTS:
  void cellular_time_slot(const cellular_time_t &T) ;
  void cellular_zone_slot(olson *tz, suggestion_t s, bool sure) ;
private Q_SLOTS:
  void unix_signal(int signo) ;
private:
  UnixSignal *signal_object ;
public:
  Q_OBJECT ;
public:
  bool notify(QObject *obj, QEvent *ev)
  {
    try { return QCoreApplication::notify(obj, ev); }
    catch(const iodata::validator::exception &e)
    {
      log_critical("%s", e.info().c_str()) ;
    }
    catch(const iodata::exception &e)
    {
      log_critical("iodata::exception: '%s'", e.info().c_str()) ;
    }
    catch(const std::exception &e)
    {
      log_critical("oops, unknown std::exception: %s", e.what()) ;
    }
    catch(...)
    {
      log_critical("oops, unknown exception of unknown type ...") ;
    }
    log_critical("aborting...") ;
    abort();
  }
} ;

#endif
