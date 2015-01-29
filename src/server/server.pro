QT -= gui
QT += dbus

TEMPLATE = app

TARGET = timed

VERSION = $$(TIMED_VERSION)

INCLUDEPATH += ../h

QMAKE_LIBDIR_FLAGS += -L../lib
LIBS += -ltimed

IODATA_TYPES = config.type settings.type customization.type tzdata.type

HEADERS += settings.h \
    csd.h \
    adaptor.h \
    timed.h \
    unix-signal.h \
    onitz.h \
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
    timed.cpp \
    timeutil.cpp \
    misc.cpp \
    settings.cpp \
    unix-signal.cpp \
    onitz.cpp \
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

xml.files  = com.nokia.time.context
xml.path = $$(DESTDIR)/usr/share/contextkit/providers

# typeinfo.files = queue.type config.type settings.type customization.type tzdata.type timed-cust-rc.type
# typeinfo.path = $$(DESTDIR)/usr/share/timed/typeinfo

timedrc.files = timed-qt5.rc
dbusconf.files = timed-qt5.conf
systemd.files = timed-qt5.service
oneshot.files = setcaps-timed-qt5.sh
statefs.files = timed-statefs.conf
statefs.path = /etc
INSTALLS += statefs

timedrc.path  = $$(DESTDIR)/etc
dbusconf.path  = $$(DESTDIR)/etc/dbus-1/system.d
systemd.path = $$(DESTDIR)/usr/lib/systemd/user
oneshot.path = $$(DESTDIR)/usr/lib/oneshot.d

INSTALLS += target xml backupconf backupscripts cud rfs timedrc dbusconf systemd oneshot

QMAKE_CXXFLAGS  += -Wall

OTHER_FILES += *.sh
