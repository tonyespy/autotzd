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

#ifndef AUTOTZD_ADAPTOR_H
#define AUTOTZD_ADAPTOR_H

#include <sys/types.h>
#include <unistd.h>

#include <sstream>

#include <QList>
#include <QString>
#include <QDBusAbstractAdaptor>
#include <QDBusMessage>

#include "autotzd.h"
#include "misc.h"
#include "csd.h"

#define SQC str().toStdString().c_str()
#define QC toStdString().c_str()
#define CC c_str()
#define PEER autotzd->peer ? autotzd->peer->info(message.service().toStdString()).c_str() : qPrintable(message.service())

static QDateTime time_t_to_qdatetime(time_t t)
{
  struct tm tm ;
  if(gmtime_r(&t, &tm) != &tm)
    return QDateTime() ;
  return QDateTime(QDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday), QTime(tm.tm_hour,tm.tm_min,tm.tm_sec), Qt::UTC) ;
}

class com_nokia_time : public QDBusAbstractAdaptor
{
  Q_OBJECT ;
  Q_CLASSINFO("D-Bus Interface", "com.nokia.time") ;
  Autotzd *autotzd ;

public:
  com_nokia_time(Autotzd *parent) : QDBusAbstractAdaptor(parent), autotzd(parent)
  {
    setAutoRelaySignals(true) ;
  }

public slots:

  bool fake_csd_time_signal(const QString &mcc, const QString &mnc, int offset, int time, int dst, int seconds, int nano_seconds)
  {
    log_notice("(fake_csd_time_signal) mcc='%s' mnc='%s' offset=%d time=%d dst=%d seconds=%d nano_seconds=%d", mcc.toStdString().c_str(), mnc.toStdString().c_str(), offset, time, dst, seconds, nano_seconds) ;
    QDateTime qdt = time_t_to_qdatetime((time_t)time) ;
    if (not qdt.isValid())
    {
      log_error("invalid time=%d parameter in in fake_csd_time_signal()", time) ;
      return false ;
    }
    NetworkTimeInfo nti(qdt, dst, offset, seconds, nano_seconds, mnc, mcc);
    log_notice("FAKE_CSD::csd_time_s %s", csd_t::csd_network_time_info_to_string(nti).c_str()) ;
    autotzd->csd->process_csd_network_time_info(nti) ;
    return true ;
  }

  bool fake_csd_time_signal_now(const QString &mcc, const QString &mnc, int offset, int time, int dst)
  {
    log_notice("(fake_csd_time_signal_now) mcc='%s' mnc='%s' offset=%d time=%d dst=%d", mcc.toStdString().c_str(), mnc.toStdString().c_str(), offset, time, dst) ;
    nanotime_t now = nanotime_t::monotonic_now() ;
    return fake_csd_time_signal(mcc, mnc, offset, time, dst, now.sec(), now.nano()) ;
  }

  bool fake_nitz_signal(int mcc, int offset, int time, int dst)
  {
    log_notice("(fake_nitz_signal) mcc=%d offset=%d time=%d dst=%d", mcc, offset, time, dst) ;
    QDateTime qdt = time_t_to_qdatetime((time_t)time) ;
    if (not qdt.isValid())
    {
      log_error("invalid time=%d parameter in in fake_nitz_signal()", time) ;
      return false ;
    }
    nanotime_t now = nanotime_t::monotonic_now() ;
    QString mcc_s = str_printf("%d", mcc).c_str() ;
    NetworkTimeInfo nti(qdt, dst, offset, now.sec(), now.nano(), "mnc", mcc_s);
    log_notice("FAKE_CSD::csd_time_s %s", csd_t::csd_network_time_info_to_string(nti).c_str()) ;
    autotzd->csd->process_csd_network_time_info(nti) ;
    return true ;
  }

  bool fake_operator_signal(const QString &mcc, const QString &mnc)
  {
    log_notice("FAKE_CSD::csd_operator_s {mcc='%s', mnc='%s'}", mcc.toStdString().c_str(), mnc.toStdString().c_str()) ;
    autotzd->csd->process_csd_network_operator(mcc, mnc) ;
    return true ;
  }
} ;

#endif /* AUTOTZD_ADAPTOR_H */
