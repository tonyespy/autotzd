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

#ifndef TIMED_NOTIFICATION_H
#define TIMED_NOTIFICATION_H

#include <QObject>
#include <QSocketNotifier>

#include "nanotime.h"

struct kernel_notification_t : public QObject
{
  QSocketNotifier *timerfd ;
  int fd ;

  bool is_running ;
  nanotime_t time_at_zero ;

  kernel_notification_t(QObject *parent = 0);
 ~kernel_notification_t() ;
  void start() ;
  void stop() ;
  Q_OBJECT ;
private Q_SLOTS:
  void ready_to_read(int fd) ;
Q_SIGNALS:
  void system_time_changed(const nanotime_t &) ;
  void restart_alarm_timer();
} ;


#endif//TIMED_NOTIFICATION_H
