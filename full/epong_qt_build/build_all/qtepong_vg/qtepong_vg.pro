#
# EPong Qt project.
#

#DEFINES += USE_GLES11
#DEFINES += USE_GLES20
DEFINES += USE_OPENVG

# debug logging
#DEFINES += EPONG_DEBUG
#DEFINES += GE_DEBUG
#DEFINES += QT_NO_DEBUG_OUTPUT

include(../../qtepong.pri)

TARGET = qtepong_vg
symbian:TARGET.UID3 = 0xef83b5a7
