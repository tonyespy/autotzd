/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.
 *
 *  Copyright (C) 2013 Jolla Ltd.
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

#ifndef AUTOTZD_NETWORKTIMEWATCHER_H
#define AUTOTZD_NETWORKTIMEWATCHER_H

#include <QObject>
#include <QVariantMap>
#include <QDBusVariant>

#include "modemwatcher.h"

class QDBusInterface;
class QDBusPendingCallWatcher;

class NetworkTimeWatcher : public ModemWatcher
{
    Q_OBJECT

public:
    explicit NetworkTimeWatcher(const QString objectPath, QObject *parent = 0);
    ~NetworkTimeWatcher();

public slots:
    void queryNetworkTime();

signals:
    void networkTimeChanged(QVariantMap map);
    void networkTimeQueryCompleted(QVariantMap map);

private slots:
    void queryNetworkTimeCallback(QDBusPendingCallWatcher *watcher);
    void onNetworkTimeChanged(QVariantMap map);
};
#endif /* AUTOTZD_NETWORKTIMEWATCHER_H */
