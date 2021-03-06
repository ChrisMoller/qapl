QT       += core gui widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += debug_and_release

LIBS += -L`apl --show_lib_dir` -lapl -lreadline
INCLUDEPATH += /home/moller/Downloads/apl-1486

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aplexec.cpp \
    chart2dwindow.cpp \
    complexspinbox.cpp \
    edif.cpp \
    greeklineedit.cpp \
    helpwindow.cpp \
    history.cpp \
    main.cpp \
    mainwindow.cpp \
    menuops.cpp \
    plot2dwindow.cpp \
    processline.cpp \
    scidoublespinbox.cpp \
    textitem.cpp \
    xml.cpp

HEADERS += \
    aplexec.h \
    chart2dwindow.h \
    complexspinbox.h \
    enums.h \
    greeklineedit.h \
    helpwindow.h \
    history.h \
    mainwindow.h \
    optionstrings.h \
    plot2ddata.h \
    plot2dwindow.h \
    processline.h \
    scidoublespinbox.h \
    textitem.h \
    xml.h \
    XMLtags.def

PREFIX = /usr/local

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin

unix {
   target.path = $$PREFIX/bin
}
   
!isEmpty(target.path): INSTALLS += target
