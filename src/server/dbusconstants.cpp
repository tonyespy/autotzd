/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.
 *
 *  Copyright 2015 Canonical Ltd.
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

#include "dbusconstants.h"

const char *DbusConstants::DBUS_PROPS_INTERFACE = "org.freedesktop.DBus.Properties";
const char *DbusConstants::DBUS_PROPS_GET_METHOD = "Get";
const char *DbusConstants::DBUS_PROPS_GETALL_METHOD = "GetAll";
const char *DbusConstants::DBUS_PROPS_CHANGED_SIGNAL = "PropertiesChanged";
