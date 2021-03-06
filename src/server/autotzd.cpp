/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.

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

#define _BSD_SOURCE

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QFile>
#include <QDateTime>
#include <QDir>

#include "config.type.h"

#include "adaptor.h"
#include "autotzd.h"
#include "tz.h"
#include "tzdata.h"
#include "csd.h"
#include "time.h"
#include "../common/log.h"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#define DEFAULT_TZONE "Europe/Helsinki"

static void spam()
{
#if NO_SPAM
  time_t now = time(NULL) ;
  for(int i=0; i<12; ++i)
  {
    time_t then = now + i*30*24*60*60 ;
    struct tm t ;
    localtime_r(&then, &t) ;
    log_debug("i=%d, time:%ld, t.tm_gmtoff=%ld", i, then, t.tm_gmtoff) ;
  }
#endif
#if NO_SPAM
  qlonglong A=1111, B=2222 ;
  QString str = "bebe " ;
  str += QString(" timestamp: %1/%2").arg(A).arg(B) ;
  log_debug("%s", str.toStdString().c_str()) ;
#endif
#if NO_SPAM
  log_info("AA") ;
  for(nanotime_t x(-2,0); x.sec()<3; x+=nanotime_t(0,100000000))
  {
    QString res ;
    QTextStream os(&res) ;
    os << x ;
    log_info("%s=%ld", string_q_to_std(res).c_str(), x.to_time_t()) ;
  }
  log_info("BB") ;
#endif
}

Autotzd::Autotzd(int ac, char **av) :
  QCoreApplication(ac, av)
{
  spam() ;

  first_boot_date_adjusted = false;
  log_debug() ;

  init_unix_signal_handler() ;
  log_debug() ;

  init_configuration() ;
  log_debug() ;

  init_default_properties() ;
  log_debug() ;

  //  init_main_interface_object() ;
  //  log_debug() ;

  //  init_main_interface_dbus_name() ;
  //  log_debug() ;

  // TODO: make autotzd wait for timedate
  // to grab TZ, before continuing.

  init_timedate_service() ;
  log_debug() ;

  init_cellular_services() ;
  log_info("daemon is up and running") ;

  /*
   * TODO(awe): for now disable this code, as it may not
   * be necessary to notify systemd that this job is ready
   * *AND* we haven't yet switched touch to systemd by
   * default for system jobs:
   *
   * if (arguments().indexOf("--systemd") >= 0) {
   *  sd_notify(0, "READY=1");
   * }
   */
}

// * Start Unix signal handling
void Autotzd::init_unix_signal_handler()
{
  signal_object = UnixSignal::object() ;
  QObject::connect(signal_object, SIGNAL(signal(int)), this, SLOT(unix_signal(int))) ;
  signal_object->handle(SIGINT) ;
  signal_object->handle(SIGTERM) ;
  signal_object->handle(SIGCHLD) ;
}

// * Reading configuration file
// * Warning if no exists (which is okey)
void Autotzd::init_configuration()
{
  iodata::storage *config_storage = new iodata::storage ;
  config_storage->set_primary_path(configuration_path()) ;
  config_storage->set_validator(etc_timed_rc_validator(), "config_t") ;

  iodata::record *c = config_storage->load() ;
  log_assert(c, "loading configuration settings failed") ;

  if(config_storage->source()==0)
    log_info("configuration loaded from '%s'", configuration_path()) ;
  else
    log_warning("configuration file '%s' corrupted or non-existing, using default values", configuration_path()) ;

  std::string data_directory = c->get("data_directory")->str();
  data_path = QDir().homePath() + QDir::separator() + QString::fromStdString(data_directory);
  if (!QDir(data_path).exists())
    QDir().mkpath(data_path);

  std::string settings_file = c->get("settings_file")->str();
  settings_path = data_path + QDir::separator() + QString::fromStdString(settings_file);

  threshold_period_long = c->get("queue_threshold_long")->value() ;
  threshold_period_short = c->get("queue_threshold_short")->value() ;
  log_debug("deleting iodata::record *c") ;
  delete c ;
  log_debug("deleting config_storage") ;
  delete config_storage ;
  log_debug("done") ;
}

static bool parse_boolean(const string &str)
{
  return str == "true" || str == "True" || str == "1" ;
}

void Autotzd::init_default_properties()
{
  format24_by_default = true;
  nitz_supported = true;
  auto_time_by_default = true;
  guess_tz_by_default = true;
  tz_by_default = DEFAULT_TZONE;
}

/*
void Autotzd::init_main_interface_object()
{
  new com_nokia_time(this) ;
  QDBusConnection conn = Maemo::Autotzd::bus() ;
  const char * const path = Maemo::Autotzd::objpath() ;
  if (conn.registerObject(path, this))
    log_info("main interface object registered on path '%s'", path) ;
  else
    log_critical("remote methods not available; failed to register dbus object: %s", Maemo::Autotzd::bus().lastError().message().toStdString().c_str()) ;
  // XXX:
  // probably it's a good idea to terminate if failed
  // (usually it means, the dbus connection is not available)
  // but on the other hand we can still provide some services (like setting time and zone)
  // Anyway, we will terminate if the mutex like name is not available
}
*/
/*
void Autotzd::init_main_interface_dbus_name()
{
  // We're misusing the dbus name as a some kind of mutex:
  //   only one instance of timed is allowed to run.
  // This is the why we can't drop the name later.

  const string conn_name = Maemo::Autotzd::bus().name().toStdString() ;
  if (Maemo::Autotzd::bus().registerService(Maemo::Autotzd::service()))
    log_info("service name '%s' registered on bus '%s'", Maemo::Autotzd::service(), conn_name.c_str()) ;
  else
  {
    const string msg = Maemo::Autotzd::bus().lastError().message().toStdString() ;
    log_critical("can't register service '%s' on bus '%s': '%s'", Maemo::Autotzd::service(), conn_name.c_str(), msg.c_str()) ;
    log_critical("aborting") ;
    ::exit(1) ;
  }
}
*/

void Autotzd::init_timedate_service()
{
  td_watcher = new TimedateWatcher;

  bool res1 = QObject::connect(this, SIGNAL(new_timezone_slot(const std::string &)), td_watcher, SLOT(new_timezone_slot(const std::string &)));
  log_assert(res1);
}

void Autotzd::init_cellular_services()
{
  tzdata::init(tz_by_default) ;
  csd = new csd_t(this) ;
  tz_oracle = new tz_oracle_t ;

  bool res1 = QObject::connect(csd, SIGNAL(csd_cellular_time(const cellular_time_t &)), this, SLOT(cellular_time_slot(const cellular_time_t &)));
  bool res2 = QObject::connect(csd, SIGNAL(csd_cellular_offset(const cellular_offset_t &)), tz_oracle, SLOT(cellular_offset(const cellular_offset_t &)));
  bool res3 = QObject::connect(csd, SIGNAL(csd_cellular_operator(const cellular_operator_t &)), tz_oracle, SLOT(cellular_operator(const cellular_operator_t &)));
  bool res4 = QObject::connect(tz_oracle, SIGNAL(cellular_zone_detected(olson *, suggestion_t, bool)), this, SLOT(cellular_zone_slot(olson *, suggestion_t, bool)));

  log_assert(res1) ;
  log_assert(res2) ;
  log_assert(res3) ;
  log_assert(res4) ;

}

Autotzd::~Autotzd()
{
  //  stop_dbus() ;
  stop_stuff() ;
}
/*
void Autotzd::stop_dbus()
{
  Maemo::Autotzd::bus().unregisterService(Maemo::Autotzd::service()) ;
  QDBusConnection::disconnectFromBus(QDBusConnection::sessionBus().name());
  QDBusConnection::disconnectFromBus(QDBusConnection::systemBus().name()) ;
}
*/
void Autotzd::stop_stuff()
{
  log_debug() ;
  delete tz_oracle ;
  log_debug() ;
  cellular_handler::uninitialize() ;
  log_notice("stop_stuff() DONE") ;
}

void Autotzd::invoke_signal(const nanotime_t &back)
{
  log_debug("systime_back=%s, back=%s", systime_back.str().c_str(), back.str().c_str()) ;
  systime_back += back ;
  log_debug("new value: systime_back=%s", systime_back.str().c_str()) ;
  if(signal_invoked)
    return ;
  signal_invoked = true ;
  int methodIndex = this->metaObject()->indexOfMethod("send_time_settings()") ;
  QMetaMethod method = this->metaObject()->method(methodIndex);
  method.invoke(this, Qt::QueuedConnection);
}

void Autotzd::unix_signal(int signo)
{
  switch(signo)
  {
    default:
      log_info("unix signal %d [%s] detected", signo, strsignal(signo)) ;
      break ;
    case SIGCHLD:
      int status ;
      while(pid_t pid = waitpid(-1, &status, WNOHANG))
      {
        if(pid==-1 && errno==EINTR)
        {
          log_info("waitpid() interrupted, retrying...") ;
          continue ;
        }
        else if(pid==-1)
        {
          if(errno!=ECHILD)
            log_error("waitpid() failed: %m") ;
          break ;
        }
        unsigned cookie = children.count(pid) ? children[pid] : 0 ;
        string name = str_printf("the child pid=%d", pid) ;
        if(cookie)
          name += str_printf(" [cookie=%d]", cookie) ;
        else
          name += " (unknown cookie)" ;
        if(WIFEXITED(status))
          log_info("%s exited with status %d", name.c_str(), WEXITSTATUS(status)) ;
        else if(WIFSIGNALED(status))
          log_info("%s killed by signal %d", name.c_str(), WTERMSIG(status)) ;
        else
        {
          log_info("%s changed status", name.c_str()) ;
          continue ;
        }
        children.erase(pid) ;
      }
      break ;
    case SIGINT:
      log_info("Keyboard interrupt, oh weh... bye") ;
      quit() ;
      break ;
    case SIGTERM:
      log_info("Termination signal... bye") ;
      quit() ;
      break ;
  }
}

void Autotzd::cellular_time_slot(const cellular_time_t &T)
{
  nanotime_t time_at_zero = nanotime_t(T.value) - T.ts ;
  log_debug("time_at_zero=%s, (T=%s)", time_at_zero.str().c_str(), T.str().c_str()) ;
}

void Autotzd::cellular_zone_slot(olson *tz, suggestion_t s, bool sure)
{
  (void) sure ;
  log_debug("time zone '%s' magicaly detected", tz->name().c_str()) ;

  // AWE: the old code never checked the new zone against the previous
  // zone.  Probably need to add a string to contain the last timezone
  // name as an autotzd member.

  emit new_timezone_slot(tz->name());
}
