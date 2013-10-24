/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef __GE_QTAUDIOOUT__
#define __GE_QTAUDIOOUT__

#include <QtCore/qobject.h>
#include <QtCore/qfile.h>
#include <qaudio.h>
#include <qaudiodeviceinfo.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstring.h>
#include <QtCore/QThread>

#include "AudioInterfaces.h"


class QAudioOutput;

namespace GF {

    class AudioOut : public QThread {
        Q_OBJECT

    public:
        AudioOut(QObject *parent, GF::IAudioSource *source);
        virtual ~AudioOut();

    /*
     * call this manually only if you are not using thread(with Symbian)
     * Note, when using GE, windowwg owning the audioout will handle of
     * calling this.
     */
     void tick();
    bool usingThead() { return m_usingThread; }

    private slots:
        // For internal notify "solution"
        void audioNotify();

    protected:
         // This is for the threaded mode only
         virtual void run();

        bool m_usingThread;
        qint64 m_samplesMixed;

        QAudioOutput *m_audioOutput;
        QIODevice *m_outTarget;
        GF::IAudioSource *m_source;
        int m_runstate;
        AUDIO_SAMPLE_TYPE *m_sendBuffer;
        int m_sendBufferSize;

    };
}

#endif
