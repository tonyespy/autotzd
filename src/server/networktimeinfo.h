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

#ifndef AUTOTZD_NETWORKTIMEINFO_H
#define AUTOTZD_NETWORKTIMEINFO_H

#include <QDateTime>
#include <QString>
#include <time.h>

class NetworkTimeInfo
{
public:
    NetworkTimeInfo(const QDateTime &dateTime, const int daylightAdjustment,
                    const int offsetFromUtc, const qlonglong timestampSeconds,
                    const qlonglong timestampNanoSeconds, const QString &mnc, const QString &mcc);
    NetworkTimeInfo(const NetworkTimeInfo &other);
    NetworkTimeInfo();
    ~NetworkTimeInfo();

    QDateTime dateTime() const;
    int offsetFromUtc() const;
    int daylightAdjustment() const;
    const timespec *timestamp() const;
    QString mnc() const;
    QString mcc() const;
    bool isValid() const;
    QString toString() const;

private:
    QDateTime m_dateTime;
    int m_daylightAdjustment;
    int m_offsetFromUtc;
    QString m_mnc;
    QString m_mcc;
    struct timespec m_timespec;
};
#endif /* AUTOTZD_NETWORKTIMEINFO_H */
