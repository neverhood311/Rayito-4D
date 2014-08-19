#-------------------------------------------------
#
# Project created by QtCreator 2012-08-23T16:57:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Rayito_Stage5_GUI
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    RaytraceMain.cpp \
    lodepng.cpp

HEADERS  += MainWindow.h \
    rayito.h \
    RMath.h \
    RRay.h \
    RMaterial.h \
    RLight.h \
    RScene.h \
    RSampling.h \
    lodepng.h \
    logger.h \
    SceneLoader.h

FORMS    += MainWindow.ui

#The next line should be uncommented for Linux
#QMAKE_CXXFLAGS += -Wno-unused-parameter

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Rsd/Rsd/x64/release/ -lRsd
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Rsd/Rsd/x64/debug/ -lRsd
else:unix: LIBS += -L$$PWD/../../Rsd/Rsd/ -lRsd

INCLUDEPATH += $$PWD/../../Rsd/include
DEPENDPATH += $$PWD/../../Rsd/include

win32:CONFIG(release, debug|release): LIBS += -LC:/local/boost_1_55_0/lib64-msvc-12.0/ -llibboost_atomic-vc120-mt-gd-1_55
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/local/boost_1_55_0/lib64-msvc-12.0/ -llibboost_atomic-vc120-mt-gd-1_55

INCLUDEPATH += C:/local/boost_1_55_0/
DEPENDPATH += C:/local/boost_1_55_0/
