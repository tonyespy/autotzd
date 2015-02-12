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

#ifndef AUTOTZD_NETWORKOPERATOR_H
#define AUTOTZD_NETWORKOPERATOR_H

#include <QObject>
#include <QMap>
#include <QVariant>

#include "ofonomodemmanager.h"

class NetworkRegistrationWatcher;

class NetworkOperator : public QObject
{
    Q_OBJECT

public:
    explicit NetworkOperator(QObject *parent = 0);
    ~NetworkOperator();
    QString mnc() const;
    QString mcc() const;
    bool isValid() const;

signals:
    void operatorChanged(const QString &mnc, const QString &mcc);

private:
    QString m_mnc;
    QString m_mcc;
    bool m_mccUpdated;
    bool m_mncUpdated;
    bool m_valid;
    QString m_currentObjectPath;
    OfonoModemManager m_modemManager;
    QMap<QString, NetworkRegistrationWatcher*> m_watcherMap;

private slots:
    void onModemAdded(QString objectPath);
    void onModemRemoved(QString objectPath);
    void onWatcherPropertyChanged(QString objectPath, QString name, QVariant value);
};

#endif /* AUTOTZD_NETWORKOPERATOR_H */
