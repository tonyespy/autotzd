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

#ifndef AUTOTZD_NETWORKREGISTRATIONRWATCHER_H
#define AUTOTZD_NETWORKREGISTRATIONRWATCHER_H

#include <QObject>
#include <QVariantMap>
#include <QDBusVariant>

#include "modemwatcher.h"

class QDBusInterface;
class QDBusPendingCallWatcher;

class NetworkRegistrationWatcher : public ModemWatcher
{
    Q_OBJECT
public:
    explicit NetworkRegistrationWatcher(const QString objectPath, QObject *parent = 0);
    ~NetworkRegistrationWatcher();

    void getProperties();

signals:
    void propertyChanged(QString objectPath, QString name, QVariant value);

private slots:
    void onPropertyChanged(QString name, QDBusVariant value);
    void getPropertiesAsync();
    void getPropertiesAsyncCallback(QDBusPendingCallWatcher *watcher);
};

#endif // AUTOTZD_NETWORKREGISTRATIONRWATCHER_H
