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

#ifndef AUTOTZD_MODEMWATCHER_H
#define AUTOTZD_MODEMWATCHER_H

#include <QObject>
#include <QVariant>
#include <QDBusVariant>

class QDBusInterface;
class QDBusPendingCallWatcher;

class ModemWatcher : public QObject
{
    Q_OBJECT

public:
    explicit ModemWatcher(const QString objectPath, const QString interface, QObject *parent = 0);
    ~ModemWatcher();
    bool interfaceAvailable() const;
    QString objectPath() const;
    QString interface() const;

signals:
    void interfaceAvailableChanged(bool available);

private:
    QString m_objectPath;
    QString m_interface;
    bool m_interfaceAvailable;
    bool checkInterfaceAvailability(QVariant variant);

private slots:
    void onModemPropertyChanged(QString objectPath, QDBusVariant value);
};
#endif /* AUTOTZD_MODEMWATCHER_H */
