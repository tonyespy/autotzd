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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <ContextProvider>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <iodata-qt5/validator>
#include <iodata-qt5/storage>
#else
#include <iodata/validator>
#include <iodata/storage>
#endif

#include "wrappers.h"
#include "singleshot.h"
#include "settings.h"
#include "unix-signal.h"
#include "onitz.h"
#include "olson.h"
#include "tz.h"
#include "csd.h"
#include "peer.h"
#include "notification.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
namespace statefs {
  namespace qt {
    class InOutWriter;
  }
}
#endif

struct Timed : public QCoreApplication
{
public:
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  inline const char *configuration_path() { return  "/etc/timed-qt5.rc" ; }
#else
  inline const char *configuration_path() { return  "/etc/timed.rc" ; }
#endif
  // inline const char *configuration_type() { return  "/usr/share/timed/typeinfo/config.type" ; }

  inline const char *customization_path() { return  "/usr/share/timed/customization.data" ; } // TODO: make it configurable
  // inline const char *customization_type() { return  "/usr/share/timed/typeinfo/customization.type" ; }

  // inline const char *settings_file_type() { return  "/usr/share/timed/typeinfo/settings.type" ; }

private:
#if 0
  bool act_dead_mode ;
#endif

  bool scratchbox_mode ;

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
  void init_dbus_peer_info() ;
  void init_scratchbox_mode() ;
  void init_act_dead() ;
  void init_configuration() ;
  void init_customization() ;
  void init_read_settings() ;
  void init_device_mode() ;
  void init_context_objects() ;
  void init_main_interface_object() ;
  void init_main_interface_dbus_name() ;
  void init_dst_checker() ;
  void init_cellular_services() ;
  void init_apply_tz_settings() ;
  void init_kernel_notification() ;

public:
  void stop_context() ;
  void stop_stuff() ;
  void stop_dbus() ;

public:

  source_settings *settings ;
  cellular_handler *nitz_object ;
#if OFONO
  csd_t *csd ;
#endif
  peer_t *peer ;

  QDBusConnectionInterface *ses_iface ;

  map<int,unsigned> children ;

public Q_SLOTS:
  void system_owner_changed(const QString &name, const QString &oldowner, const QString &newowner) ;
  void register_child(unsigned cookie, int pid) { children[pid] = cookie ; }
  void session_reported(const QString &address) ;
Q_SIGNALS:
  void settings_changed(const Maemo::Timed::WallClock::Info &, bool system_time) ;
  void next_bootup_event(int next_boot_event, int next_non_boot_event);
  void alarm_triggers_changed(Maemo::Timed::Event::Triggers);
  // void settings_changed_1(bool system_time) ;
public:
  Timed(int ac, char **av) ;
  virtual ~Timed() ;
  int get_default_gmt_offset() { return default_gmt_offset ; }

private:
  iodata::storage *event_storage, *settings_storage ;

  simple_timer *short_save_threshold_timer, *long_save_threshold_timer ;
  unsigned threshold_period_long, threshold_period_short ;
  QString data_path, events_path, settings_path;
  int default_gmt_offset ;
  std::string current_mode ;
  void load_rc() ;
  void load_settings() ;
public:
  void save_settings() ;
private:
  Q_INVOKABLE void save_event_queue() ;

  Q_INVOKABLE void send_time_settings() ;
  bool signal_invoked ;
  nanotime_t systime_back ;
  QTimer *dst_timer ;
  std::string sent_signature ;
  tz_oracle_t *tz_oracle ;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  statefs::qt::InOutWriter *alarm_present;
  statefs::qt::InOutWriter *alarm_trigger;
#else
  ContextProvider::Property *time_operational_p ;
  ContextProvider::Property *alarm_present;
  ContextProvider::Property *alarm_trigger;
  ContextProvider::Service *context_service ;
#endif

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
  void harmattan_init_done(int runlevel) ;
  void harmattan_desktop_visible() ;
  void kernel_notification(const nanotime_t &jump_forwards) ;
  void restart_alarm_timer();
  void set_alarm_present(bool present);
  void set_alarm_trigger(const QMap<QString, QVariant> &triggers);
public:
#if 0
  void device_mode_reached(bool act_dead, const std::string &dbus_session) ;
#endif
  void device_mode_reached(bool user_mode) ;
#if 0
  void nitz_notification(const cellular_info_t &) ;
  void tz_by_oracle(olson *tz, tz_suggestions_t) ;
#endif
public Q_SLOTS:
  void check_dst() ;
public:
  void update_oracle_context(bool set) ;
  void open_epoch() ;
  void halt(const string &what) { halted = what ; exit(0) ; } // exit the main loop
  string is_halted() { return halted ; }
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
