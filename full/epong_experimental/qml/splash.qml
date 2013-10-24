import QtQuick 1.0

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
            Qt.quit();
        }
    }

    Image
    {
        id:splashimage
        source:"default.png"
        fillMode:Image.PreserveAspectCrop
        anchors.fill: parent
        smooth: true
        opacity: 0 /* initially not visible */
    }


    SequentialAnimation {
       id:anim
       ParallelAnimation {
           PropertyAnimation {
               target: splashimage
               duration: 3000
               properties: "opacity"
               from:0
               to:1
               easing.type: Easing.Linear
           }
           PropertyAnimation {
               target: splashimage
               duration: 3000
               properties: "scale"
               from:3.0
               to:1.0
               easing.type: Easing.OutExpo
           }
       }
       onCompleted: {
          Qt.quit();
       }
    }

    Component.onCompleted: {
        console.log("splash.qml loaded");
        anim.running=true;
    }
}
