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

#ifndef AUTOTZD_ONITZ_H
#define AUTOTZD_ONITZ_H

#include <string>
#include <sstream>
using namespace std ;

#include <QObject>
#include <QDateTime>

#include "../common/log.h"

# include "networktime.h"
# include "networkoperator.h"
# include "networktimeinfo.h"

#include "nanotime.h"

#include "misc.h"
#include "cellular.h"

struct cellular_handler : public QObject
{
  Q_OBJECT ;
private:
  static cellular_handler *static_object ;
  virtual ~cellular_handler() ;
  cellular_handler() ;

signals:

public:
  static cellular_handler *object() ;
  static void uninitialize() ;
  void fake_nitz_signal(int mcc, int offset, int time, int dst) ;
public slots:
} ;

#endif /* AUTOTZD_ONITTZ_H */
