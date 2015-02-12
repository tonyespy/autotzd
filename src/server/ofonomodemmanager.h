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

#ifndef AUTOTZD_OFONOMODEMMANAGER_H
#define AUTOTZD_OFONOMODEMMANAGER_H

#include <QDBusObjectPath>
#include <QObject>
#include <QStringList>
#include <QVariant>

class ModemWatcher;

class OfonoModemManager : public QObject
{
    Q_OBJECT

public:
    explicit OfonoModemManager(QObject *parent = 0);
    ~OfonoModemManager();

    QStringList getModems();

signals:
    void modemAdded(QString objectPath);
    void modemRemoved(QString objectPath);

private slots:
    void onModemAdded(QDBusObjectPath objectPath, QVariantMap map);
    void onModemRemoved(QDBusObjectPath objectPath);

private:
    QStringList m_modemList;
    bool addModem(QString objectPath);
};

#endif /* AUTOTZD_OFONOMODEMMANAGER_H */
