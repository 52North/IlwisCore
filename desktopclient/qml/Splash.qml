import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1


Rectangle {
    anchors.fill : parent
    property int count : 0
    color : "transparent"
    property string version : "Alpha 1"
    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: {
              first.state = "end"
        }
    }

    StartBlock{
        id : first
        x :  0 //rootwindow.width / 4
        y : 0 //rootwindow.height / 4
        width : 272
        height : 201
        color : "red"
        endX : rootwindow.width / 2 - first.width
        endY : rootwindow.height / 2  - first.height
        Image {
            anchors.fill: parent
            source: "images/lefttop.png"
        }


        onRotationChanged: {
            if ( count != 0 && rotation == 0){
                rootwindow.initIlwis()
            }
            ++count
        }
    }

    StartBlock{
        x : rootwindow.width - first.width
        y : 0
        width : first.width
        height : first.height
        color : "blue"
        endX : rootwindow.width / 2
        endY : rootwindow.height / 2 - first.height
        state : first.state
        Image {
            anchors.fill: parent
            source: "images/righttop.png"
        }


    }

    StartBlock{
        x : rootwindow.width - first.width
        y : rootwindow.height - first.height
        width : first.width
        height : first.height
        color : "green"
        endX : rootwindow.width / 2
        endY : rootwindow.height / 2
        state : first.state
        Image {
            anchors.fill: parent
            source: "images/rightbottom.PNG"
        }
    }

    StartBlock{
        x : 0
        y : rootwindow.height - first.height
        width : first.width
        height : first.height
        color : "yellow"
        endX : rootwindow.width / 2 - first.width
        endY : rootwindow.height / 2
        state : first.state
        Image {
            anchors.fill: parent
            id: name
            source: "images/leftbottom.png"
        }
    }
    function buildString() {
        return "201604251305"
    }

    Column {
        id : message
        z : 100
        y : 50 + parent.height / 2
        x : 280
        height : 60
        width : parent.width / 2
        opacity : first.opacity

        Text {
            id : txt
            width : parent.width
            height : 30
            text : version
            font.pointSize: 18
            color : "blue"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
            width : parent.width
            height : 30
            text : "Build : " + buildString()
            font.pointSize: 12
            color : "darkblue"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}


