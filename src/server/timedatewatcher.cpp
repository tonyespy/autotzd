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

#include <QDBusInterface>
#include <QDBusReply>

#include "../common/log.h"

#include "dbusconstants.h"
#include "timedateconstants.h"
#include "timedatewatcher.h"

TimedateWatcher::TimedateWatcher(QObject *parent) :
    QObject(parent)
{
    QDBusConnection::systemBus().connect(TimedateConstants::TIMEDATE_SERVICE,
                                         TimedateConstants::TIMEDATE_PATH,
                                         DbusConstants::DBUS_PROPS_INTERFACE, "PropertiesChanged",
                                         this, SLOT(onTimedatePropertiesChanged(QString, QDBusVariant)));

    QDBusMessage request = QDBusMessage::createMethodCall(TimedateConstants::TIMEDATE_SERVICE,
                                                          TimedateConstants::TIMEDATE_PATH,
                                                          DbusConstants::DBUS_PROPS_INTERFACE,
                                                          "GetAll");
    QList<QVariant> args;
    args.append("org.freedesktop.timedate1");
    request.setArguments(args);

    // TODO: make this Async!!!
    QDBusReply<QVariantMap> reply = QDBusConnection::systemBus().call(request);
    if (reply.error().isValid()) {
        log_error("DBus call to interface %s function GetAll of path %s failed: %s",
                  DbusConstants::DBUS_PROPS_INTERFACE,
                  TimedateConstants::TIMEDATE_PATH,
                  reply.error().message().toStdString().c_str());
    } else {
        QVariantMap map = reply.value();
        foreach (const QString &key, map.keys())
	    // log_debug("property: %s value: %s", key.toStdString().c_str(), map.value(key));
            log_debug("property: %s", key.toStdString().c_str());
    }
}

TimedateWatcher::~TimedateWatcher()
{
    QDBusConnection::systemBus().disconnect(TimedateConstants::TIMEDATE_SERVICE,
                                            TimedateConstants::TIMEDATE_PATH,
                                            DbusConstants::DBUS_PROPS_INTERFACE, "PropertiesChanged",
                                            this, SLOT(onTimedatePropertiesChanged(QString, QDBusVariant)));
}

void TimedateWatcher::onTimedatePropertiesChanged(QString name, QDBusVariant value)
{
}
