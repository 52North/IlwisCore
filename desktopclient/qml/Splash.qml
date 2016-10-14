import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import StartIlwis 1.0


Rectangle {
    id : container
    property int count : 0
    width : 0
    height : 0
    Timer {
         running: true
        onTriggered: {
            state = "end"
        }
    }

    property int time : 2000
    state : "begin"

    Image{
        id : first
        width : parent.width
        height : parent.height
        source : "images/splash.png"

    }


    Column {
        id : message
        z : 100
        y : 50 + parent.height / 2
        x : 40
        height : 60
        width : parent.width / 2
        opacity : first.opacity

        Text {
            id : txt
            width : parent.width
            height : 30
            text : startilwis.buildType() + " " + (startilwis.verionNumber() != "0" ? startilwis.verionNumber() : "")
            font.pointSize: 18
            color : "blue"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
            width : parent.width
            height : 30
            text : "Build : " + startilwis.buildNumber()
            font.pointSize: 12
            color : "darkblue"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
    states : [
        State {
            name : "end"
            PropertyChanges {target: container ; width : 543 }
            PropertyChanges {target: container ; height : 401}
             PropertyChanges {target: message ; opacity : 1}

        },
        State {
            name : "begin"
            PropertyChanges {target: container ; width : 0 }
            PropertyChanges {target: container ; height : 0}
            PropertyChanges {target: message ; opacity : 0}
        }
    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "width"; duration : time ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "height"; duration : time ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : time ; easing.type: Easing.InQuart}

            onRunningChanged: {
                if ((state == "end") && (!running)) {
                    rootwindow.initIlwis()
                }
            }
        }
    ]
}



