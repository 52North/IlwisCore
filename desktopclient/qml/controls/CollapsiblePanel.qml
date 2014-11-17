import QtQuick 2.1

Rectangle {
    id: contextPanel
    property int panelHeight
    property color backgroundColor : "white"

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


    property alias titleText: titleTxt.text


    Rectangle {
        id: titleRect
        width: parent.width
        height: 24
        color : background2
        BorderImage{

            width: parent.width
            height: 24
            border { left: 15; top: 0; right: 15; bottom: 0 }
            source : "../images/headerblue2CS1.png"
            smooth : true
            Image {
                id: titleArrow
                width:  15
                height:  15
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                source: "../images/arrowdown.png"

            }
            Text{
                id: titleTxt
                anchors.left: titleArrow.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                // color:  "DarkBlue"
                font.bold: true
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
