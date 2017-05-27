import QtQuick 2.1

import "../Global.js" as Global

Rectangle {
    id: contextPanel
    property int panelHeight
    property color backgroundColor : Global.mainbackgroundcolor
    property color headerColor : uicontext.workbenchBGColor
    property int headerHeight : 24
    property alias titleText: titleTxt.text
    property var expandableArea : internealArea
    property int fontsize : 12
    property bool boldfont : true
    property string arrowtype : "arrowdown.png"

    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "lightgrey";
        }
        GradientStop {
            position: 1.00;
            color: "white";
        }
    }


    Rectangle {
        id: titleRect
        width: parent.width
        height: headerHeight
        color : headerColor
        Image {
            id: titleArrow
            width:  15
            height:  15
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            source: "../images/" + arrowtype

        }
        Text{
            id: titleTxt
            anchors.left: titleArrow.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: fontsize;
            // color:  "DarkBlue"
            font.bold: boldfont
        }
        MouseArea{
            width: parent.width
            height:  parent.height
            onClicked: {
                if (contextPanel.state == "collapsed")
                {
                    contextPanel.state = "open"
                }
                else
                {
                    contextPanel.state = "collapsed"
                }
            }
        }
    }

    Rectangle {
        id : internealArea
        width: parent.width
        anchors.top: titleRect.bottom
        opacity : 0
        color : backgroundColor
        clip : true
    }

    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: internealArea
                height: 0
                opacity: 0
            }
            PropertyChanges {
                target: titleArrow
                rotation: -90
            }
        },
        State{
            name: "open"
            PropertyChanges {
                target: internealArea
                height: panelHeight
                opacity: 1
            }
            PropertyChanges {
                target: titleArrow
                rotation: 0
            }
            PropertyChanges {
                target: contextPanel
                height : panelHeight + titleRect.height
            }
        }

    ]

    transitions: [
        Transition {
            NumberAnimation { target: contextPanel; property: "height"; duration: 300 }
            NumberAnimation { target: internealArea; property: "height"; duration: 300 }
            NumberAnimation { target: internealArea; property: "opacity"; duration: 300 }
            NumberAnimation { target: titleArrow; property: "rotation"; duration: 300 }
        }
    ]

    Component.onCompleted:  {
        contextPanel.height = titleRect.height+ ( state == "open"? panelHeight : 0)
    }
}
