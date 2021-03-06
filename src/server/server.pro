QT -= gui
QT += dbus

TEMPLATE = app

TARGET = autotzd

INCLUDEPATH += ../h

IODATA_TYPES = config.type tzdata.type

HEADERS += csd.h \
    adaptor.h \
    autotzd.h \
    unix-signal.h \
    onitz.h \
    nanotime.h \
    networktime.h \
    networktimewatcher.h \
    networkoperator.h \
    networkregistrationwatcher.h \
    networktimeinfo.h \
    ofonomodemmanager.h \
    modemwatcher.h \
    ofonoconstants.h \
    dbusconstants.h \
    timedateconstants.h \
    timedatewatcher.h

SOURCES += tzdata.cpp \
    cellular.cpp \
    csd.cpp \
    main.cpp \
    autotzd.cpp \
    timeutil.cpp \
    misc.cpp \
    unix-signal.cpp \
    onitz.cpp \
    nanotime.cpp \
    networktime.cpp \
    networktimewatcher.cpp \
    networkoperator.cpp \
    networkregistrationwatcher.cpp \
    networktimeinfo.cpp \
    ofonomodemmanager.cpp \
    modemwatcher.cpp \
    ofonoconstants.cpp \
    dbusconstants.cpp \
    timedateconstants.cpp \
    timedatewatcher.cpp

SOURCES += olson.cpp tz.cpp
HEADERS += tz.h

CONFIG += link_pkgconfig
#PKGCONFIG += libpcrecpp libsystemd-daemon

CONFIG += iodata

target.path = $$(DESTDIR)/usr/bin

autotzd.files = timed-qt5.rc
dbusconf.files = timed-qt5.conf
systemd.files = timed-qt5.service

autotzdrc.files = timed.rc
autotzrc.path  = $$(DESTDIR)/etc
dbusconf.path  = $$(DESTDIR)/etc/dbus-1/system.d
systemd.path = $$(DESTDIR)/usr/lib/systemd/user

INSTALLS += target dbusconf systemd

QMAKE_CXXFLAGS  += -Wall

OTHER_FILES += *.sh
