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
#ifndef TIMED_H
#define TIMED_H

#include <QCoreApplication>
#include <QMetaMethod>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>

#include <iodata/validator.h>
#include <iodata/storage.h>

#include "wrappers.h"
#include "settings.h"
#include "unix-signal.h"
#include "onitz.h"
#include "olson.h"
#include "tz.h"
#include "csd.h"
#include "notification.h"

struct Timed : public QCoreApplication
{
public:
  inline const char *configuration_path() { return  "/etc/timed-qt5.rc" ; }
  inline const char *customization_path() { return  "/usr/share/timed/customization.data" ; } // TODO: make it configurable

private:

  bool format24_by_default ;
  bool auto_time_by_default ;
  bool guess_tz_by_default ;

  bool nitz_supported ;
  string tz_by_default ;
  bool first_boot_date_adjusted;

public:
  bool is_nitz_supported() { return nitz_supported ; }
  const string &default_timezone() { return tz_by_default ; }
  const QString get_settings_path() { return settings_path; }
  void init_first_boot_hwclock_time_adjustment_check();

private:

  // init_* methods, to be called by constructor only
  void init_unix_signal_handler() ;
  void init_configuration() ;
  void init_customization() ;
  void init_read_settings() ;
  void init_main_interface_object() ;
  void init_main_interface_dbus_name() ;
  void init_cellular_services() ;
  void init_apply_tz_settings() ;
  void init_kernel_notification() ;

public:
  void stop_stuff() ;
  void stop_dbus() ;

public:

  source_settings *settings ;
  cellular_handler *nitz_object ;
  csd_t *csd ;

  QDBusConnectionInterface *ses_iface ;

  map<int,unsigned> children ;

public Q_SLOTS:
  void system_owner_changed(const QString &name, const QString &oldowner, const QString &newowner) ;
  void register_child(unsigned cookie, int pid) { children[pid] = cookie ; }
  void session_reported(const QString &address) ;
Q_SIGNALS:
  void settings_changed(const Maemo::Timed::WallClock::Info &, bool system_time) ;
  void next_bootup_event(int next_boot_event, int next_non_boot_event);
public:
  Timed(int ac, char **av) ;
  virtual ~Timed() ;
  int get_default_gmt_offset() { return default_gmt_offset ; }

private:
  iodata::storage *event_storage, *settings_storage ;

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
  kernel_notification_t *notificator ;
  void invoke_signal(const nanotime_t &) ;
  void invoke_signal() { nanotime_t zero=0 ; invoke_signal(zero) ; }
  void clear_invokation_flag() { signal_invoked = false ; systime_back.set(0) ; }
public Q_SLOTS:
  void event_queue_changed() ;
private Q_SLOTS:
  void queue_threshold_timeout() ;
  void unix_signal(int signo) ;
  void kernel_notification(const nanotime_t &jump_forwards) ;
public:
  void update_oracle_context(bool set) ;
private:
  string halted ;
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
