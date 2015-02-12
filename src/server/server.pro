QT -= gui
QT += dbus

TEMPLATE = app

TARGET = autotzd

VERSION = $$(TIMED_VERSION)

INCLUDEPATH += ../h

#QMAKE_LIBDIR_FLAGS += -L../lib
#LIBS += -ltimed

IODATA_TYPES = config.type settings.type customization.type tzdata.type

HEADERS += settings.h \
    csd.h \
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
    ofonoconstants.h

SOURCES += tzdata.cpp \
    cellular.cpp \
    csd.cpp \
    main.cpp \
    autotzd.cpp \
    timeutil.cpp \
    misc.cpp \
    settings.cpp \
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
    ofonoconstants.cpp

SOURCES += olson.cpp tz.cpp
HEADERS += tz.h

SOURCES += notification.cpp
HEADERS += notification.h

CONFIG += link_pkgconfig
#PKGCONFIG += libpcrecpp libsystemd-daemon

CONFIG += iodata

target.path = $$(DESTDIR)/usr/bin

# typeinfo.files = queue.type config.type settings.type customization.type tzdata.type timed-cust-rc.type
# typeinfo.path = $$(DESTDIR)/usr/share/timed/typeinfo

autotzd.files = timed-qt5.rc
dbusconf.files = timed-qt5.conf
systemd.files = timed-qt5.service

autotzdrc.files = timed.rc
autotzrc.path  = $$(DESTDIR)/etc
dbusconf.path  = $$(DESTDIR)/etc/dbus-1/system.d
systemd.path = $$(DESTDIR)/usr/lib/systemd/user

INSTALLS += target backupconf backupscripts cud rfs timedrc dbusconf systemd

QMAKE_CXXFLAGS  += -Wall

OTHER_FILES += *.sh
