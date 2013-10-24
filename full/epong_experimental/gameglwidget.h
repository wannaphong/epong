#ifndef GAMEGLWIDGET_H
#define GAMEGLWIDGET_H

#include <QGLWidget>
#include "GameApp.h"

#ifdef USE_GOOM_MONITOR
#include <goommonitorsession.h>
#include <goommonitorplugin.hrh>
#endif


class GameGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GameGLWidget(GF::GameApp *game, QWidget *parent = 0);
    virtual ~GameGLWidget();

    void uninitializeGL();
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
signals:

public slots:

protected:
    GF::GameApp *game;
#ifdef USE_GOOM_MONITOR
    RGOomMonitorSession iGoom;
#endif

    bool gamePrepared;


};

#endif // GAMEGLWIDGET_H
