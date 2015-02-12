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

#include "networktimeinfo.h"

NetworkTimeInfo::NetworkTimeInfo(const QDateTime &dateTime,
                                 const int daylightAdjustment,
                                 const int offsetFromUtc,
                                 const qlonglong timestampSeconds,
                                 const qlonglong timestampNanoSeconds,
                                 const QString &mnc,
                                 const QString &mcc)
    : m_dateTime(dateTime), m_daylightAdjustment(daylightAdjustment),
      m_offsetFromUtc(offsetFromUtc),
      m_mnc(mnc), m_mcc(mcc)
{
    m_timespec.tv_sec = timestampSeconds;
    m_timespec.tv_nsec = timestampNanoSeconds;
}

NetworkTimeInfo::NetworkTimeInfo(const NetworkTimeInfo &other)
{
    m_dateTime = other.dateTime();
    m_daylightAdjustment = other.daylightAdjustment();
    m_offsetFromUtc = other.offsetFromUtc();
    m_timespec.tv_sec = other.timestamp()->tv_sec;
    m_timespec.tv_nsec = other.timestamp()->tv_nsec;
    m_mnc = other.mnc();
    m_mcc = other.mcc();
}

NetworkTimeInfo::NetworkTimeInfo()
{
    memset(&m_timespec, 0, sizeof(struct timespec));
}

NetworkTimeInfo::~NetworkTimeInfo()
{
}

QDateTime NetworkTimeInfo::dateTime() const
{
    return m_dateTime;
}

int NetworkTimeInfo::offsetFromUtc() const
{
    return m_offsetFromUtc;
}

int NetworkTimeInfo::daylightAdjustment() const
{
    return m_daylightAdjustment;
}

const timespec* NetworkTimeInfo::timestamp() const
{
    return &m_timespec;
}

QString NetworkTimeInfo::mnc() const
{
    return m_mnc;
}

QString NetworkTimeInfo::mcc() const
{
    return m_mcc;
}

bool NetworkTimeInfo::isValid() const
{
    return m_dateTime.isValid();
}

QString NetworkTimeInfo::toString() const
{
    if (!m_dateTime.isValid())
        return QString("Invalid");

    return QString("%1, UTC offset: %2, DST: %3, MNC: %4, MCC: %5, Received: %6.%7")
            .arg(m_dateTime.toString())
            .arg(m_offsetFromUtc)
            .arg(m_daylightAdjustment)
            .arg(m_mnc)
            .arg(m_mcc)
            .arg(m_timespec.tv_sec)
            .arg(m_timespec.tv_nsec);
}
