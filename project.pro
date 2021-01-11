QT       += core gui sql multimedia printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 c++14

unix:LIBS += -linsane
unix:INCLUDEPATH = /usr/lib/x86_64-linux-gnu

win32:LIBS += -L"C:\msys64\mingw64\bin" -linsane
win32:INCLUDEPATH = C:\msys64\mingw64\include

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    caseinfo.cpp \
    caseinfoview.cpp \
    casesmain.cpp \
    casesmainview.cpp \
    clientsform.cpp \
    clientsformview.cpp \
    customcalendarwidget.cpp \
    customdateedit.cpp \
    docs.cpp \
    docsview.cpp \
    eventtracking.cpp \
    eventtrackingview.cpp \
    fizclient.cpp \
    fizclientview.cpp \
    imageviewer.cpp \
    main.cpp \
    mainwindow.cpp \
    newevent.cpp \
    neweventview.cpp \
    popup.cpp \
    reminder.cpp \
    reporter.cpp \
    scaner.cpp \
    scanerview.cpp \
    yurclient.cpp \
    yurclientview.cpp

HEADERS += \
    casecatdelegate.h \
    caseinfo.h \
    caseinfoview.h \
    casesmain.h \
    casesmainview.h \
    clientsform.h \
    clientsformview.h \
    clstatusdelegate.h \
    customcalendarwidget.h \
    customdateedit.h \
    dateformatdelegate.h \
    docs.h \
    docsview.h \
    enums.h \
    eventcatdelegate.h \
    eventtracking.h \
    eventtrackingview.h \
    fizclient.h \
    fizclientview.h \
    imageviewer.h \
    mainwindow.h \
    namedelegate.h \
    newevent.h \
    neweventview.h \
    popup.h \
    reminder.h \
    reporter.h \
    scaner.h \
    scanerview.h \
    yurclient.h \
    yurclientview.h

FORMS += \
    caseinfoview.ui \
    casesmainview.ui \
    clientsformview.ui \
    docsview.ui \
    eventtrackingview.ui \
    fizclientview.ui \
    imageviewer.ui \
    mainwindow.ui \
    neweventview.ui \
    reporter.ui \
    scanerview.ui \
    yurclientview.ui

TRANSLATIONS += \
    project_ru_RU.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resource.qrc
