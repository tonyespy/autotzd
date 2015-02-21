/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.
 *
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

#ifndef AUTOTZD_TIMEDATEWATCHER_H
#define AUTOTZD_TIMEDATEWATCHER_H

#include <QObject>
#include <QVariant>
#include <QDBusVariant>

class QDBusInterface;
class QDBusPendingCallWatcher;

class TimedateWatcher : public QObject
{
    Q_OBJECT

public:
    explicit TimedateWatcher(QObject *parent = 0);
    ~TimedateWatcher();

public Q_SLOTS:
    void new_timezone_slot(const std::string &timezone);

private slots:
    void onTimedatePropertiesChanged(QString objectPath, QDBusVariant value, QStringList invalidProps);
};
#endif /* AUTOTZD_TIMEDATEWATCHER_H */
