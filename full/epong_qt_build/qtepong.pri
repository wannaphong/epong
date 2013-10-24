#
# EPong Qt project.
#

TARGET = qtepong
TEMPLATE = app

QT       += core gui
CONFIG += mobility
MOBILITY = systeminfo

#DEFINES += QTGAMEENABLER_USE_VOLUME_HACK # Symbian-specific, crashes with Symbian^3 Belle
#DEFINES += TRIAL_VERSION

# audio related options
DEFINES += USE_OGG
DEFINES += USE_PUSH_AUDIO_OUT

# rendering options
#DEFINES += USE_GLES11
#DEFINES += USE_GLES20
#DEFINES += USE_OPENVG

# debug logging
#DEFINES += EPONG_DEBUG
#DEFINES += GE_DEBUG
#DEFINES += QT_NO_DEBUG_OUTPUT

INCLUDEPATH += $$PWD/../game_engine/ $$PWD/../epong_game_logic/

# Game sources
SOURCES += \
    $$PWD/../epong_game_logic/PongApp.cpp \
    $$PWD/../epong_game_logic/stb_vorbis.c \
    $$PWD/../epong_game_logic/PongGame.cpp \
    $$PWD/../epong_game_logic/PongObject.cpp \
    $$PWD/../epong_game_logic/ParticleEngine.cpp \
    $$PWD/../epong_game_logic/FontWriter.cpp \
    $$PWD/../epong_game_logic/GameMenu.cpp \
    $$PWD/../epong_game_logic/PongBackground.cpp \
    $$PWD/../epong_game_logic/PongHighScores.cpp \
    $$PWD/../epong_game_logic/PongCredits.cpp \
    $$PWD/../epong_game_logic/MusicPlayer.cpp
HEADERS += \
    $$PWD/../epong_game_logic/PongApp.h \
    $$PWD/../epong_game_logic/PongGame.h \
    $$PWD/../epong_game_logic/PongObject.h \
    $$PWD/../epong_game_logic/ParticleEngine.h \
    $$PWD/../epong_game_logic/FontWriter.h \
    $$PWD/../epong_game_logic/GameMenu.h \
    $$PWD/../epong_game_logic/PongBackground.h \
    $$PWD/../epong_game_logic/PongHighScores.h \
    $$PWD/../epong_game_logic/PongCredits.h \
    $$PWD/../epong_game_logic/MusicPlayer.h \
    $$PWD/../epong_game_logic/stb_vorbis.h

# Game engine
SOURCES += \
    $$PWD/../game_engine/GameEngine.cpp \
    $$PWD/../game_engine/GameApp.cpp \
    $$PWD/../game_engine/AudioInterfaces.cpp \
    $$PWD/../game_engine/AudioBuffer.cpp \
    $$PWD/../game_engine/eglutils.cpp \
    $$PWD/../game_engine/audiosourceif.cpp
HEADERS += \
    $$PWD/../game_engine/GameEngine.h \
    $$PWD/../game_engine/GameApp.h \
    $$PWD/../game_engine/AudioInterfaces.h \
    $$PWD/../game_engine/AudioBuffer.h \
    $$PWD/../game_engine/eglutils.h \
    $$PWD/../game_engine/audiosourceif.h \
    $$PWD/../game_engine/audioout.h


contains(DEFINES, USE_GLES11) {
SOURCES += $$PWD/../game_engine/eglutilsgles.cpp
HEADERS += $$PWD/../game_engine/eglutilsgles.h
}
contains(DEFINES, USE_GLES20) {
SOURCES += $$PWD/../game_engine/eglutilsgles.cpp
HEADERS += $$PWD/../game_engine/eglutilsgles.h
}
contains(DEFINES, USE_OPENVG) {
SOURCES += $$PWD/../game_engine/eglutilsvg.cpp
HEADERS += $$PWD/../game_engine/eglutilsvg.h
}


# Qt implementation of the engine
SOURCES += $$PWD/main.cpp \
    $$PWD/QtGameEngine.cpp \
    $$PWD/GEAudioOut.cpp \
    $$PWD/accelerometerfilter.cpp \
    $$PWD/pushaudioout.cpp \
    $$PWD/pullaudioout.cpp \

HEADERS += \
    $$PWD/QtGameEngine.h \
    $$PWD/GEAudioOut.h \
    $$PWD/accelerometerfilter.h \
    $$PWD/pushaudioout.h \
    $$PWD/pullaudioout.h \
    $$PWD/geglobal.h \
    $$PWD/trace.h \
    $$PWD/geglobal.h

RESOURCES += \
    $$PWD/resources.qrc



include($$PWD/../spritebatchqt/spritebatch.pri)


unix:!symbian {
        message("Linux")
        !contains(MEEGO_EDITION,harmattan) {
            QT += multimedia
        }
        #only needed in unix desktop
        message($$QMAKESPEC)
        linux-g++-* {
            message("The project contains desktop ogl")
            message($$INCLUDEPATH)
            message($$LIBS)
            LIBS += -lX11 -lEGL -lGLESv2
        }
}

maemo5: {
    QT += multimedia
    LIBS += -lX11 -lEGL -lGLESv2
}

windows: {
}


symbian: {
    # FOR ENABLING HARDWARE FLOATS (old SBSv1 style)
    #MMP_RULES += "OPTION gcce -march=armv6"
    #MMP_RULES += "OPTION gcce -mfpu=vfp"
    #MMP_RULES += "OPTION gcce -mfloat-abi=softfp"
    #MMP_RULES += "OPTION gcce -marm"

    # VFP magic. For enabling hardware floats.
    QMAKE_CXXFLAGS+= -O3 -marm -march=armv6 -mfpu=vfp -mfloat-abi=softfp

    TARGET.UID3 = 0xef83b5a5

    QT += multimedia
    CONFIG += mobility
    MOBILITY += sensors feedback
    # Feedback for haptic.

    # To handle volume up / down keys on Symbian
    LIBS += -lremconcoreapi
    LIBS += -lremconinterfacebase

    LIBS += -llibEGL

    LIBS += -lcone -leikcore -lavkon

    # FOR UGLY VOLUMEHACK
    INCLUDEPATH += /epoc32/include/mmf/common
    INCLUDEPATH += /epoc32/include/mmf/server
    LIBS += -lmmfdevsound

    # ICON is qtepong.svg by default, but because of multi-pro builds, set working directory
    ICON=$$PWD/qtepong.svg


    # TARGET.CAPABILITY +=
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000

	# comment the following line and the corresponding end to use iap
    iap {

# Notice: To get the IAP working, you must
#  - install debug message handler with qInstallMsgHandler or make a pause between IAPClient instantation and usage
#  - declare the IAPClient::ProductDataList as metatype (outside any function declaration) with
#      Q_DECLARE_METATYPE(IAPClient::ProductDataList)  //to be able to transfer data this type with Qt::QueuedConnection
#  - Register (at most once?) the meta type before the IAPClient is created with:
#      qRegisterMetaType<IAPClient::ProductDataHash>("IAPClient::ProductDataHash");
# See main.cpp and ../epong_iap_impl/miniiapqtclient.cpp


        CONFIG += iapppurchasing
        MOBILITY += serviceframework

        TARGET.CAPABILITY += NetworkServices
        LIBS += -lcaf \
            -lcafutils \
            -lapmime

        DEFINES += IAP
        SOURCES += \
        $$PWD/../epong_iap_impl/miniiapqtclient.cpp \
        $$PWD/../epong_iap_impl/drmfile.cpp \
        $$PWD/../epong_iap_impl/miniiapinterface.cpp \
        $$PWD/../epong_iap_impl/miniiapclientapi.cpp \
        $$PWD/../epong_iap_impl/Symbian/drmfile_p.cpp
        HEADERS += \
        $$PWD/../epong_iap_impl/miniiapinterface.h \
        $$PWD/../epong_iap_impl/miniiapclientapi.h \
        $$PWD/../epong_iap_impl/miniiapqtclient.h \
        $$PWD/../epong_iap_impl/epong_iap_constants.h \
        $$PWD/../epong_iap_impl/drmfile.h \
        $$PWD/../epong_iap_impl/Symbian/drmfile_p.h

		# to build "real" IAP, the UID3 should not start with 0xE and the TEST_MODE.txt should not be deployed 
		# (the end of the line should be commented)
        addConfigFiles.sources = $$PWD/../epong_iap_impl/Data/IAP_VARIANTID.txt $$PWD/../epong_iap_impl/Data/TEST_MODE.txt
        addConfigFiles.path = .
        DEPLOYMENT += addConfigFiles

        addDrm_642613.sources = $$PWD/../epong_iap_impl/Data/2player.dat
        addDrm_642613.path = drm/data/resourceid_642613
        DEPLOYMENT += addDrm_642613

        debug: {
            DEFINES += IAP_DEBUG
            MMP_RULES -= "PAGED"
            MMP_RULES += "UNPAGED"
        }
    } # iap block ends

}

contains(MEEGO_EDITION,harmattan) {

    LIBS += -lEGL

    CONFIG += x11
    INCLUDEPATH += "/usr/include"
    DEFINES += MY_OS_MEEGO
    # For new GE classes
    DEFINES += MEEGO_EDITION_HARMATTAN
    CONFIG += mobility
    MOBILITY += sensors feedback
    MOBILITY += multimedia
    message(MEEGO HARMATTAN)
    # Classify the application as a game to support volume keys on Harmattan.
    gameclassify.files += $$PWD/qtc_packaging/qtepong.conf
    gameclassify.path = /usr/share/policy/etc/syspart.conf.d
    # from the /usr/share/qt4/mkspecs/features/qt.prf,
    # you can see the magic word for QtMeeGoGraphicsSystemHelper lib.
    # Currently, this is not available in Qt SDK
    QT += meegographicssystemhelper
    INSTALLS += gameclassify
    # VFP magic. For enabling hardware floats.
    QMAKE_CXXFLAGS+= -O3 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=hard
    CONFIG += qmsystem2
    RESOURCES += $$PWD/meego_resources.qrc

    OTHER_FILES += \
        $$PWD/qtc_packaging/debian_fremantle/rules \
        $$PWD/qtc_packaging/debian_fremantle/README \
        $$PWD/qtc_packaging/debian_fremantle/copyright \
        $$PWD/qtc_packaging/debian_fremantle/control \
        $$PWD/qtc_packaging/debian_fremantle/compat \
        $$PWD/qtc_packaging/debian_fremantle/changelog \
        $$PWD/qtc_packaging/debian_harmattan/rules \
        $$PWD/qtc_packaging/debian_harmattan/README \
        $$PWD/qtc_packaging/debian_harmattan/copyright \
        $$PWD/qtc_packaging/debian_harmattan/control \
        $$PWD/qtc_packaging/debian_harmattan/compat \
        $$PWD/qtc_packaging/debian_harmattan/changelog \
        $$PWD/qtc_packaging/qtepong.conf

}

# Add more folders to ship with the application, here
#folder_01.source = ../epong_sounds/*.wav
#folder_01.target = ./
#folder_02.source = ../epong_sounds/*.ogg
#folder_02.target = ./
#DEPLOYMENTFOLDERS = folder_01 folder_02

# will append files matched by <folder_01.source> (either files or files in folder recursively)
# into QCoreApplication::applicationDirPath() + "/<folder_01.target>" in Symbian,
# and QCoreApplication::applicationDirPath() + "/../<folder_01.target>" in Linux and in MeeGo.
# Specifically, in MeeGo, the path will be /opt/qtepong/./
# See QmlApplicationViewer::adjustPath, which is feeded
# a string of the form <folder_01.target>/<filename> (e.g., qml/main.qml) to
# get a path of the corresponding file.

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()



