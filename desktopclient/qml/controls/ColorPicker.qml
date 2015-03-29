import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global


Rectangle {
    id:comboBox
    property variant items: ["a","b","c","de", "f","ff"];
    property alias selectedItem: chosenItemText.text;
    property string lefttopColor : "blue"
    property string leftbottomColor : "green"
    property string righttopColor : "red"
    property string rightbottomColor : "white"
    signal comboClicked;
    //            width: 100;
    //            height: 30;
    z: 100;
    smooth:true;
    color : "transparent"

    Rectangle {
        id:chosenItem
        radius:2;
        width:parent.width;
        height:comboBox.height;
        color: "lightgrey"
        border.width: 1
        border.color: "#9494B8"
        smooth:true;
        Text {
            width: parent.width-50; height: parent.height
            anchors.left: parent.left; anchors.leftMargin: 10;
            verticalAlignment: Text.AlignVCenter
            id:chosenItemText
            text:comboBox.items[0];
            font.family: "Arial"
            font.pixelSize: 12;
            smooth:true
        }

        Item {
            width: 20; height: 20
            anchors.right: parent.right
            Image {
                width: 16; height:16;
                anchors.centerIn: parent;
                source: "../images/down20.png"
                fillMode: Image.PreserveAspectFit
            }
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                comboBox.state = comboBox.state==="dropDown"?"":"dropDown"
            }
        }
    }

    Rectangle {
        id:dropDown
        width:comboBox.width;
        height:0;
        clip:true;
        radius:4;
        anchors.top: chosenItem.bottom;
        anchors.margins: 1;
        color: "lightgrey"
        border.width: 1
        border.color: "#B0B0B0"

        Rectangle {
            id:listView
            height:240
            width : parent.width
            color : Global.alternatecolor1
            Button{
                id : lefttopButton
                width : 25
                height : 14
                Rectangle{
                    anchors.fill: parent
                    anchors.margins: 3
                    color : lefttopColor
                }
            }

            Column{
                id : grid
                anchors.left: lefttopButton.right
                anchors.top : lefttopButton.top
                y : 30
                x : 30
                Repeater{
                    id : columns
                    model : 10

                    Row {
                        id : rowDelegate
                        property int currentCol : index
                        Repeater{
                            id : rows
                            model : 10
                            Rectangle{
                                height : 14
                                width : 14
                                border.width: 1
                                color : Qt.rgba(index/10,rowDelegate.currentCol/10,0,1)
                                border.color: dropDown.color
                            }
                        }
                    }
                }
            }
            Button{
                id : righttopButton
                width : 25
                height : 14
                anchors.left : grid.right
                anchors.top : lefttopButton.top
                Rectangle{
                    anchors.fill: parent
                    anchors.margins: 3
                    color : righttopColor
                }
            }
            Button{
                id : leftbottomButton
                width : 25
                height : 14
                anchors.left : lefttopButton.left
                anchors.bottom: grid.bottom
                Rectangle{
                    anchors.fill: parent
                    anchors.margins: 3
                    color : leftbottomColor
                }
            }
            Button{
                id : rightbottomButton
                width : 25
                height : 14
                anchors.left : grid.right
                anchors.bottom: grid.bottom
                Rectangle{
                    anchors.fill: parent
                    anchors.margins: 3
                    color : rightbottomColor
                }
            }


            border.color: Global.edgecolor

        }
    }

    Component {
        id: highlightBar
        Rectangle {
            width:comboBox.width;
            height:comboBox.height;
            color: "lightsteelblue"; radius: 5
            y: listView.currentItem.y
            Behavior on y {
                SpringAnimation {
                    spring: 3
                    damping: 0.2
                }
            }
        }
    }


    states: State {
        name: "dropDown";
        PropertyChanges { target: dropDown; height:240 }
    }

    transitions: Transition {
        NumberAnimation { target: dropDown; properties: "height"; easing.type: Easing.OutExpo; duration: 400 }
    }
}


