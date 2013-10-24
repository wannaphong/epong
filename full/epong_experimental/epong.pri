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
    $$PWD/../game_engine/audiosourceif.cpp

HEADERS += \
    $$PWD/../game_engine/GameEngine.h \
    $$PWD/../game_engine/GameApp.h \
    $$PWD/../game_engine/AudioInterfaces.h \
    $$PWD/../game_engine/AudioBuffer.h \
    $$PWD/../game_engine/audiosourceif.h \
    $$PWD/../game_engine/audioout.h

