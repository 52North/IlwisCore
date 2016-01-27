import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1


Item {
    anchors.fill : parent

    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: first.state = "end"
    }

    StartBlock{
        id : first
        x :  0 //rootwindow.width / 4
        y : 0 //rootwindow.height / 4
        width : 273
        height : 200
        color : "red"
        endX : rootwindow.width / 2 - first.width
        endY : rootwindow.height / 2  - first.height + 1
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
        endX : rootwindow.width / 2 - 1
        endY : rootwindow.height / 2 - first.height + 1
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
        endX : rootwindow.width / 2 - 2
        endY : rootwindow.height / 2 -1
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
}


