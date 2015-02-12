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

#ifndef AUTOTZD_NETWORKTIME_H
#define AUTOTZD_NETWORKTIME_H

#include <QMap>
#include <QObject>
#include <QVariant>

#include "networktimeinfo.h"
#include "ofonomodemmanager.h"

class QDBusPendingCallWatcher;
class NetworkTimeWatcher;

class NetworkTime : public QObject
{
    Q_OBJECT

public:
    explicit NetworkTime (QObject *parent=0);
    ~NetworkTime();
    NetworkTimeInfo timeInfo() const;
    void queryTimeInfo();
    bool isValid() const;

signals:
    void timeInfoChanged(const NetworkTimeInfo &timeInfo);
    void timeInfoQueryCompleted(const NetworkTimeInfo &timeInfo);

private:
    NetworkTimeInfo m_networkTimeInfo;
    QMap<QString, NetworkTimeWatcher*> m_watcherMap;
    NetworkTimeInfo parseNetworkTimeInfoFromMap(QVariantMap map);
    OfonoModemManager m_modemManager;

private slots:
    void onModemAdded(QString objectPath);
    void onModemRemoved(QString objectPath);
    void networkTimeChanged(QVariantMap map);
    void networkTimeQueryCompletedSlot(QVariantMap map);
};
#endif /* AUTOTZD_NETWORKTIME_H */
