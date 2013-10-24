#
# EPong Qt project.
#

TARGET = qtglepong
TEMPLATE = app

QT       += core gui opengl

CONFIG += mobility
MOBILITY = systeminfo

QT += multimedia



#DEFINES += TRIAL_VERSION

# enable splash screen (QML)
DEFINES += USE_QML

# audio related options
DEFINES += USE_OGG
#DEFINES += USE_PUSH_AUDIO_OUT
DEFINES += USE_PULL_AUDIO_OUT

# rendering options
DEFINES += USE_QTGL
#DEFINES += USE_GLES11
#DEFINES += USE_GLES20
#DEFINES += USE_OPENVG

# debug logging
#DEFINES += EPONG_DEBUG
#DEFINES += GE_DEBUG
#DEFINES += QT_NO_DEBUG_OUTPUT

#-----------------------------------------------------------

SOURCES += $$PWD/qglgameengine.cpp $$PWD/main.cpp \ 
    gameglwidget.cpp
HEADERS += $$PWD/qglgameengine.h \
    gameglwidget.h

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../epong_qt_build


contains(DEFINES, USE_PUSH_AUDIO_OUT) {
    SOURCES += \
        $$PWD/../epong_qt_build/GEAudioOut.cpp \
        $$PWD/../epong_qt_build/pushaudioout.cpp
    HEADERS += \
        $$PWD/../epong_qt_build/GEAudioOut.h \
        $$PWD/../epong_qt_build/pushaudioout.h
}

contains(DEFINES, USE_PULL_AUDIO_OUT) {
    SOURCES += \
        $$PWD/../epong_qt_build/GEAudioOut.cpp \
        $$PWD/../epong_qt_build/pullaudioout.cpp
    HEADERS += \
        $$PWD/../epong_qt_build/GEAudioOut.h \
        $$PWD/../epong_qt_build/pullaudioout.h
}




contains(DEFINES, USE_QML) {
    QT += declarative
}

RESOURCES += \
    $$PWD/resources.qrc

include($$PWD/epong.pri)
include($$PWD/../spritebatchqt/spritebatch.pri)

windows: {
    # Use virtual keyboard, .. for harmattan port.
    # DEFINES += USE_VKB
}

symbian: {
    MOBILITY += sensors feedback
    TARGET.UID3 = 0xef835555
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 10000000 32000000

    # Symbian
    SOURCES += $$PWD/../epong_s3_build/src/memprofile.cpp
    HEADERS += $$PWD/../epong_s3_build/inc/memprofile.h

    # set to reserve goom memory (requires copying ../goomapi contents to your Qt symbian sdk)
    #DEFINES += USE_GOOM_MONITOR
    #contains(DEFINES, USE_GOOM_MONITOR)
    #{
    #    LIBS += -lgoommonitor
    #}
}

OTHER_FILES += \
    qml/default.png \
    qml/splashvideo.qml \
    qml/splash.qml \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog




contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/epong_qtgl/bin
    QT -= multimedia
    MOBILITY += multimedia
    DEFINES += USE_VKB
    #target.path = /opt/epongqtgl/bin
    INSTALLS += target
}
