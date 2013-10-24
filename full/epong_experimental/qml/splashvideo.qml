import QtQuick 1.0
import QtMultimediaKit 1.1

Rectangle {
    id: page
    anchors.fill: parent
    width: 640;
    height: 360
    color: "black"

    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("onClicked");
            if(!video.playing) Qt.quit();
            video.stop();
        }
    }

    Image
    {
        id:splashimage
        source:"default.png"
        fillMode:Image.PreserveAspectCrop
        anchors.fill: parent
        opacity:1
    }


    Video {
        id:video
        anchors.fill: parent
        source: "rtsp://v4.cache7.c.youtube.com/CjYLENy73wIaLQnUkNgzsVpj7xMYJCAkFEIJbXYtZ29vZ2xlSARSBXdhdGNoYNrmo5ChqLzFTQw=/0/0/0/video.3gp"
        playing: false
        visible: false
        fillMode: Video.PreserveAspectFit
        onErrorChanged:{
            console.log("error "+error);
            if(playing && error!=0) Qt.quit();
        }

        onStatusChanged: {
            console.log("status "+status);
            if(status==Video.Buffered)
            {
                video.visible=true;
            }
        }

        onPlayingChanged: {
            console.log("playing "+playing);
            if(!playing)
            {
                video.visible=false;
                Qt.quit();
            }
        }
    }


    Image
    {
        visible: video.visible
        opacity: 0.75
        id:splashimage2
        source:"qtepong64.png"
        x: 480
        y: 32
    }


    Timer
    {
        running:true
        interval: 2000
        onTriggered: {
            console.log("triggered "+video.error);

            if(video.error!=0)
                Qt.quit();
            else
                video.play();
        }
    }

}
