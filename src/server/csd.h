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

#ifndef AUTOTZD_CSD_H
#define AUTOTZD_CSD_H

#include <string>

#include <QTimer>

#include "networktime.h"
#include "networkoperator.h"
#include "networktimeinfo.h"

#include "nanotime.h"

#include "cellular.h"

class Autotzd ;

struct csd_t : public QObject
{
  Q_OBJECT ;
  static const nanotime_t old_nitz_threshold ;
  static const int operator_wait_ms = 1000 ;
  Autotzd *autotzd ;
  NetworkTime *nt ;
  NetworkOperator *op ;
  Q_INVOKABLE void csd_operator_q() ;
  QTimer *timer ;
  cellular_time_t *time ;
  cellular_offset_t *offs ;
  cellular_operator_t oper ;
Q_SIGNALS:
  void csd_cellular_time(const cellular_time_t) ;
  void csd_cellular_offset(const cellular_offset_t) ;
  void csd_cellular_operator(const cellular_operator_t) ;
private Q_SLOTS:
  void csd_time_q(const NetworkTimeInfo &nti) ;
  void csd_time_s(const NetworkTimeInfo &nti) ;
  void csd_operator_s(const QString &mnc, const QString &mcc) ;
  void wait_for_operator_timeout() ;
private:
  void process_csd_network_time_info(const NetworkTimeInfo &nti) ;
  void process_csd_network_operator(const QString &mcc, const QString &mnc) ;
  friend class com_nokia_time ; // these private functions can be used by dbus fake
private:
  void input_csd_network_time_info(const NetworkTimeInfo &nti) ;
  void output_csd_network_time_info() ;

public:
  csd_t(Autotzd *owner) ;
  static std::string csd_network_time_info_to_string(const NetworkTimeInfo &nti) ;
  virtual ~csd_t() ;
} ;

#endif /* AUTOTZD_CSD_H */
