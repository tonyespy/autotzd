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
#define _BSD_SOURCE

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <systemd/sd-daemon.h>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QFile>
#include <QDateTime>
#include <QDir>

#include "queue.type.h"
#include "config.type.h"
#include "customization.type.h"
#include "settings.type.h"

#include "adaptor.h"
#include "timed.h"
#include "settings.h"
#include "tz.h"
#include "tzdata.h"
#include "csd.h"
#include "notification.h"
#include "time.h"
#include "../common/log.h"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

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

Timed::Timed(int ac, char **av) :
  QCoreApplication(ac, av)
{
  spam() ;
  halted = "" ; // XXX: remove it, as we don't want to halt anymore
  first_boot_date_adjusted = false;
  log_debug() ;

  init_unix_signal_handler() ;
  log_debug() ;

  init_configuration() ;
  log_debug() ;

  init_customization() ;
  log_debug() ;

  init_read_settings() ;
  log_debug() ;

  init_main_interface_object() ;
  log_debug() ;

  init_main_interface_dbus_name() ;
  log_debug() ;

  init_kernel_notification();

  init_first_boot_hwclock_time_adjustment_check();
  log_debug() ;

  init_cellular_services() ;

  log_debug() ;

  init_dst_checker() ;

  log_debug("applying time zone settings") ;

  init_apply_tz_settings() ;
  log_debug() ;

  log_info("daemon is up and running") ;

  if (arguments().indexOf("--systemd") >= 0) {
      sd_notify(0, "READY=1");
  }
}

// * Start Unix signal handling
void Timed::init_unix_signal_handler()
{
  signal_object = UnixSignal::object() ;
  QObject::connect(signal_object, SIGNAL(signal(int)), this, SLOT(unix_signal(int))) ;
  signal_object->handle(SIGINT) ;
  signal_object->handle(SIGTERM) ;
  signal_object->handle(SIGCHLD) ;
}

// * Reading configuration file
// * Warning if no exists (which is okey)
void Timed::init_configuration()
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

// * read customization data provided by customization package
void Timed::init_customization()
{
  iodata::storage *storage = new iodata::storage ;
  storage->set_primary_path(customization_path()) ;
  storage->set_validator(customization_data_validator(), "customization_t") ;

  iodata::record *c = storage->load() ;
  log_assert(c, "loading customization settings failed") ;

  if(storage->source()==0)
    log_info("customization loaded from '%s'", customization_path()) ;
  else
    log_warning("customization file '%s' corrupted or non-existing, using default values", customization_path()) ;

  format24_by_default = parse_boolean(c->get("format24")->str()) ;
  nitz_supported = parse_boolean(c->get("useNitz")->str()) ;
  auto_time_by_default = parse_boolean(c->get("autoTime")->str()) ;
  guess_tz_by_default = parse_boolean(c->get("guessTz")->str()) ;
  tz_by_default = c->get("tz")->str() ;

  if (not nitz_supported and auto_time_by_default)
  {
    log_warning("automatic time update disabled because nitz is not supported in the device") ;
    auto_time_by_default = false ;
  }

  delete c ;
  delete storage ;
}

// * read settings
// * apply customization defaults, if needed
void Timed::init_read_settings()
{
  settings_storage = new iodata::storage ;
  settings_storage->set_primary_path(settings_path.toStdString());
  settings_storage->set_secondary_path(settings_path.toStdString() + ".bak");
  settings_storage->set_validator(settings_data_validator(), "settings_t") ;

  iodata::record *tree = settings_storage->load() ;

  log_assert(tree, "loading settings failed") ;

#define apply_cust(key, val)  do { if (tree->get(key)->value() < 0) tree->add(key, val) ; } while(false)
  apply_cust("format_24", format24_by_default) ;
  apply_cust("time_nitz", auto_time_by_default) ;
  apply_cust("local_cellular", guess_tz_by_default) ;
#undef apply_cust

  settings = new source_settings(this) ; // TODO: use tz_by_default here
  settings->load(tree, tz_by_default) ;

  delete tree ;
}

void Timed::init_main_interface_object()
{
  new com_nokia_time(this) ;
  QDBusConnection conn = Maemo::Timed::bus() ;
  const char * const path = Maemo::Timed::objpath() ;
  if (conn.registerObject(path, this))
    log_info("main interface object registered on path '%s'", path) ;
  else
    log_critical("remote methods not available; failed to register dbus object: %s", Maemo::Timed::bus().lastError().message().toStdString().c_str()) ;
  // XXX:
  // probably it's a good idea to terminate if failed
  // (usually it means, the dbus connection is not available)
  // but on the other hand we can still provide some services (like setting time and zone)
  // Anyway, we will terminate if the mutex like name is not available
}

void Timed::init_main_interface_dbus_name()
{
  // We're misusing the dbus name as a some kind of mutex:
  //   only one instance of timed is allowed to run.
  // This is the why we can't drop the name later.

  const string conn_name = Maemo::Timed::bus().name().toStdString() ;
  if (Maemo::Timed::bus().registerService(Maemo::Timed::service()))
    log_info("service name '%s' registered on bus '%s'", Maemo::Timed::service(), conn_name.c_str()) ;
  else
  {
    const string msg = Maemo::Timed::bus().lastError().message().toStdString() ;
    log_critical("can't register service '%s' on bus '%s': '%s'", Maemo::Timed::service(), conn_name.c_str(), msg.c_str()) ;
    log_critical("aborting") ;
    ::exit(1) ;
  }
}

void Timed::init_cellular_services()
{
  tzdata::init(tz_by_default) ;
  csd = new csd_t(this) ;
  tz_oracle = new tz_oracle_t ;

  bool res1 = QObject::connect(csd, SIGNAL(csd_cellular_time(const cellular_time_t &)), settings, SLOT(cellular_time_slot(const cellular_time_t &)));
  bool res2 = QObject::connect(csd, SIGNAL(csd_cellular_offset(const cellular_offset_t &)), tz_oracle, SLOT(cellular_offset(const cellular_offset_t &)));
  bool res3 = QObject::connect(csd, SIGNAL(csd_cellular_operator(const cellular_operator_t &)), tz_oracle, SLOT(cellular_operator(const cellular_operator_t &)));
  bool res4 = QObject::connect(tz_oracle, SIGNAL(cellular_zone_detected(olson *, suggestion_t, bool)), settings, SLOT(cellular_zone_slot(olson *, suggestion_t, bool)));

  log_assert(res1) ;
  log_assert(res2) ;
  log_assert(res3) ;
  log_assert(res4) ;

}

void Timed::init_dst_checker()
{
  log_debug() ;
  dst_timer = new QTimer ;
  dst_timer->setSingleShot(true) ;
  QObject::connect(dst_timer, SIGNAL(timeout()), this, SLOT(check_dst())) ;
}

void Timed::init_apply_tz_settings()
{
  settings->postload_fix_manual_zone() ;
  settings->postload_fix_manual_offset() ;
  if(settings->check_target(settings->etc_localtime()) != 0)
    invoke_signal() ;
}

Timed::~Timed()
{
  stop_context() ;
  stop_dbus() ;
  stop_stuff() ;
}
void Timed::stop_context()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  delete context_service ;
  delete time_operational_p ;
#endif
}
void Timed::stop_dbus()
{
  Maemo::Timed::bus().unregisterService(Maemo::Timed::service()) ;
  QDBusConnection::disconnectFromBus(QDBusConnection::sessionBus().name());
  QDBusConnection::disconnectFromBus(QDBusConnection::systemBus().name()) ;
}
void Timed::stop_stuff()
{
  delete settings ;
  log_debug() ;
  delete settings_storage ;
  log_debug() ;
  delete tz_oracle ;
  log_debug() ;
  delete dst_timer ;
  log_debug() ;
  cellular_handler::uninitialize() ;
  log_notice("stop_stuff() DONE") ;
}

void Timed::system_owner_changed(const QString &name, const QString &oldowner, const QString &newowner)
{
  log_debug() ;
}

/*
 * xxx
 * These are the "stupid and simple" backup methods.
 * Just like the doctor ordered. :)
 * The chmod is a workaround for backup-framework crash bug.
 */
void Timed::save_settings()
{
  iodata::record *tree = settings->save() ;
  int res = settings_storage->save(tree) ;

  if(res==0) // primary file
    log_info("wall clock settings written") ;
  else if(res==1)
    log_warning("wall clock settings written to secondary file") ;
  else
    log_critical("wall clock settings can't be saved") ;

  delete tree ;
}

void Timed::invoke_signal(const nanotime_t &back)
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
  log_assert(q_pause==NULL) ;
}

void Timed::send_time_settings()
{
  log_debug() ;
  log_debug("settings=%p", settings) ;
  log_debug("settings->cellular_zone=%p", settings->cellular_zone) ;
  log_debug("settings->cellular_zone='%s'", settings->cellular_zone->zone().c_str()) ;
  nanotime_t diff = systime_back ;
  clear_invokation_flag() ;
  save_settings() ;
  settings->fix_etc_localtime() ;
  sent_signature = dst_signature(time(NULL)) ;
  Maemo::Timed::WallClock::Info info(settings->get_wall_clock_info(diff)) ;
  log_notice("sending signal 'settings_changed': %s", info.str().toStdString().c_str()) ;
  emit settings_changed(info, not diff.is_zero()) ;
  log_notice("signal 'settings_changed' sent") ;
  // emit settings_changed_1(systime) ;
  am->reshuffle_queue(diff) ;
  if(q_pause)
  {
    delete q_pause ;
    q_pause = NULL ;
  }
  check_dst() ; // reschedule dst timer
}

void Timed::check_dst()
{
  dst_timer->stop() ;
  time_t t = time(NULL) ;
  string signature = dst_signature(t) ;
  if (signature != sent_signature)
  {
    invoke_signal() ;
    return ;
  }

  int look_forward = 3600 ; // 1 hour
  string signature_2 = dst_signature(t+look_forward) ;
  if (signature_2 == signature)
  {
    dst_timer->start(1000*(look_forward-60)) ; // 1 minute less
    return ;
  }

  int a=0, b=look_forward ;
  while (b-a > 1)
  {
    int c = (a+b) / 2 ;
    (signature==dst_signature(t+c) ? a : b) = c ;
  }
  // now 'a' is the time until the last 'old time' second
  //     'b=a+1' until the first 'new time' second
  dst_timer->start(1000*b) ;
}

void Timed::unix_signal(int signo)
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

void Timed::update_oracle_context(bool s)
{
  log_debug("update_oracle_context(%d): NOT IMPLEMENTED", s);
}

void Timed::open_epoch()
{
  am->open_epoch() ;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  time_operational_p->setValue(true) ;
#endif
}

void Timed::session_reported(const QString &new_address)
{
  (void)new_address ;
}

void Timed::init_kernel_notification()
{
  notificator = new kernel_notification_t(this);
  QObject::connect(notificator, SIGNAL(system_time_changed(const nanotime_t &)), this, SLOT(kernel_notification(const nanotime_t &))) ;
  notificator->start() ;
}

void Timed::kernel_notification(const nanotime_t &jump_forwards)
{
  log_notice("KERNEL: system time changed by %s", jump_forwards.str().c_str()) ;
  settings->process_kernel_notification(jump_forwards) ;
}

void Timed::init_first_boot_hwclock_time_adjustment_check() {
    if (first_boot_date_adjusted)
        return;

    QString path = data_path + QDir::separator() + "first-boot-hwclock.dat";
    QFile file(path);
    if (file.exists()) {
        first_boot_date_adjusted = true;
        return;
    }

    if (QDate::currentDate().year() < 2013) {
        log_info("first boot, updating old date from year %d to 01/01/2013", QDate::currentDate().year());
        settings->set_system_time(1357041600); // January 1, 12:00:00, 2013
    }

    first_boot_date_adjusted = true;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        log_error("Failed to open file %s", path.toStdString().c_str());
        return;
    }
    if (!file.isWritable()) {
        log_error("File not writable: %s", path.toStdString().c_str());
        return;
    }

    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString() << "\n";
    file.close();
}
