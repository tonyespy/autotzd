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
#include "misc.h"
#include "timedateconstants.h"
#include "timedatewatcher.h"

TimedateWatcher::TimedateWatcher(QObject *parent) :
    QObject(parent)
{
    QDBusConnection::systemBus().connect(TimedateConstants::TIMEDATE_SERVICE,
                                         TimedateConstants::TIMEDATE_PATH,
                                         DbusConstants::DBUS_PROPS_INTERFACE,
                                         DbusConstants::DBUS_PROPS_CHANGED_SIGNAL,
                                         this, SLOT(onTimedatePropertiesChanged(QString, QDBusVariant)));

    QDBusMessage request = QDBusMessage::createMethodCall(TimedateConstants::TIMEDATE_SERVICE,
                                                          TimedateConstants::TIMEDATE_PATH,
                                                          DbusConstants::DBUS_PROPS_INTERFACE,
                                                          DbusConstants::DBUS_PROPS_GET_METHOD);

    QList<QVariant> args;
    args.append("org.freedesktop.timedate1");
    args.append("Timezone");
    request.setArguments(args);

    // TODO: make this Async!!!
    QDBusReply<QVariant> reply = QDBusConnection::systemBus().call(request);
    if (reply.error().isValid()) {
        log_error("DBus call to interface %s function Get of path %s failed: %s",
                  DbusConstants::DBUS_PROPS_INTERFACE,
                  TimedateConstants::TIMEDATE_PATH,
                  reply.error().message().toStdString().c_str());
    } else {
	QVariant timezone = reply.value();

        log_debug("Timezone: %s", timezone.toString().toStdString().c_str());
    }
}

TimedateWatcher::~TimedateWatcher()
{
    QDBusConnection::systemBus().disconnect(TimedateConstants::TIMEDATE_SERVICE,
                                            TimedateConstants::TIMEDATE_PATH,
                                            DbusConstants::DBUS_PROPS_INTERFACE,
                                            DbusConstants::DBUS_PROPS_CHANGED_SIGNAL,
                                            this, SLOT(onTimedatePropertiesChanged(QString, QDBusVariant)));
}

void TimedateWatcher::new_timezone_slot(const std::string &timezone) {
    log_debug("Timezone: %s", timezone.c_str());

    QDBusMessage request = QDBusMessage::createMethodCall(TimedateConstants::TIMEDATE_SERVICE,
                                                          TimedateConstants::TIMEDATE_PATH,
                                                          TimedateConstants::TIMEDATE_INTERFACE,
                                                          TimedateConstants::TIMEDATE_SET_TZ_METHOD);

    QList<QVariant> args;
    args.append(string_std_to_q(timezone));
    args.append(false);
    request.setArguments(args);

    // TODO: make this Async!!!
    QDBusMessage reply = QDBusConnection::systemBus().call(request);

    if (reply.errorMessage().isEmpty() == false ||
        reply.errorName().isEmpty() == false) {

        log_error("DBus call to interface %s function SetTimezone of path %s failed: %s",
                  TimedateConstants::TIMEDATE_INTERFACE,
                  TimedateConstants::TIMEDATE_PATH,
                  reply.errorMessage().toStdString().c_str());
    }
}

void TimedateWatcher::onTimedatePropertiesChanged(QString name, QDBusVariant value, QStringList invalidProps)
{
    // TODO(AWE): add code to handle Timezone PropertiesChange
    // Also, the current signature of this function is incorrect, it
    // should be QVariantMap changedProps, QArray/List invalidProps)
    log_debug();
}
