#-------------------------------------------------
#
# Project created by QtCreator 2019-11-27T20:49:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileSearch
TEMPLATE = app
# 生成目录
DESTDIR  = ../bin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
    dialogHelp.cpp \
        searchWindow.cpp \
    dialogWait.cpp \
    dialogDelete.cpp \
    searchImpl.cpp

HEADERS  += searchWindow.h \
    dialogHelp.h \
    dialogWait.h \
    msgBox.hpp \
    dialogDelete.h \
    searchConst.hpp \
    searchImpl.h

FORMS    += searchWindow.ui \
    dialogDelete.ui \
    dialogHelp.ui

BUILD_OUT_DIR="$$OUT_PWD"
CONFIG(debug, debug|release): BUILD_OUT_DIR="$$OUT_PWD/debug"
CONFIG(release, debug|release):BUILD_OUT_DIR+="$$OUT_PWD/release"
message(BUILD_OUT_DIR: $$BUILD_OUT_DIR)

# 编译后拷贝
QMAKE_POST_LINK += xcopy /Y /C "\"$$PWD/img"\" "\"$$PWD/bin/img\\"\"

# 给exe文件添加图标
RC_ICONS = img/icon.ico
