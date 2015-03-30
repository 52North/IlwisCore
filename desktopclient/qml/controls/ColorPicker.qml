import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../Global.js" as Global


Rectangle {
    id:comboBox
    property alias selectedItem: chosenItemText.text;
    property color selectedColor : calcColor(ys,xs)
    property color lefttopColor : "blue"
    property color leftbottomColor : "yellow"
    property color righttopColor : "red"
    property color rightbottomColor : "black"
    property int numberOfCells : 10
    property int xs : -1
    property int ys : -1
    signal comboClicked;
    width : 195
    height : Global.rowHeight
    z: 100;
    smooth:true;
    Layout.minimumWidth: 195
    color : "transparent"

    Rectangle {
        id:chosenItem
        radius:2;
        width:parent.width;
        height:comboBox.height;
        color: Global.alternatecolor3
        border.width: 1
        border.color: "#9494B8"
        smooth:true;
        Rectangle {
            anchors.left: parent.left
            anchors.leftMargin: 5
            height: parent.height - 4
            width : 16
            color : selectedColor
            y : 2
        }

        Text {
            width: parent.width-50; height: parent.height
            anchors.left: parent.left; anchors.leftMargin: 25;
            verticalAlignment: Text.AlignVCenter
            id:chosenItemText
            font.family: "Arial"
            font.pixelSize: 12;
            text : selectedColor
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
        color: Global.alternatecolor2
        border.width: 1
        border.color: "#B0B0B0"

        property int currentButton : 0

        Rectangle {
            id:listView
            height:160
            width : parent.width
            color : Global.alternatecolor3
            Button{
                id : lefttopButton
                width : 25
                height : 14
                onClicked: {
                    dropDown.currentButton = 0
                   if (leftColorList.enabled){
                       leftColorList.state = "invisible"
                   }else
                       leftColorList.state = "visible"
                }

                Rectangle{
                    anchors.fill: parent
                    anchors.margins: 3
                    color : lefttopColor
                }
            }
            Connections {
                 target: leftColorList
                 onColorChanged: {
                     if ( dropDown.currentButton == 0)
                        lefttopColor = leftColorList.model[leftColorList.currentColor]
                     if ( dropDown.currentButton == 2)
                        leftbottomColor = leftColorList.model[leftColorList.currentColor]
                 }
             }
            Connections {
                 target: rightColorList
                 onColorChanged: {
                     if ( dropDown.currentButton == 1)
                        righttopColor = rightColorList.model[rightColorList.currentColor]
                     if ( dropDown.currentButton == 3)
                        rightbottomColor = rightColorList.model[rightColorList.currentColor]
                 }
             }

            ColorList{
                id : leftColorList
                anchors.top: lefttopButton.bottom
                anchors.bottom : leftbottomButton.top
                anchors.left : parent.left
                anchors.margins: 2
                width : lefttopButton.width
                model : uicontext.colorNames
                state : "invisible"
            }


            Column{
                id : grid
                anchors.left: lefttopButton.right
                anchors.top : lefttopButton.top

                y : 30
                x : 30
                Repeater{
                    id : columns
                    model : numberOfCells

                    Row {
                        id : rowDelegate
                        property int currentCol : index
                        Repeater{
                            id : rows
                            model : numberOfCells
                            Rectangle{
                                height : 14
                                width : 14
                                border.width: (xs === currentCol && ys === index) ? 2 : 1
                                color : calcColor(index, currentCol)
                                border.color: (xs === currentCol && ys === index) ?  "black" : dropDown.color
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        xs = currentCol
                                        ys = index
                                        comboBox.state = ""
                                    }
                                }
                            }
                        }
                    }
                }
            }

            ColorList{
                id : rightColorList
                anchors.top: righttopButton.bottom
                anchors.bottom : rightbottomButton.top
                anchors.left : grid.right
                anchors.margins: 2
                width : righttopButton.width
                model : uicontext.colorNames
                state : "invisible"
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
                onClicked: {
                    dropDown.currentButton = 1
                   if (rightColorList.enabled){
                       rightColorList.state = "invisible"
                   }else
                       rightColorList.state = "visible"
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
                onClicked: {
                    dropDown.currentButton = 2
                   if (leftColorList.enabled){
                       leftColorList.state = "invisible"
                   }else
                       leftColorList.state = "visible"
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
                onClicked: {
                    dropDown.currentButton = 3
                   if (rightColorList.enabled){
                       rightColorList.state = "invisible"
                   }else
                       rightColorList.state = "visible"
                }
            }


            border.color: Global.edgecolor

        }
    }

    function calcColor(x, y){
        if ( x === -1 || y === -1)
            return "transparent"

        var rD1 = Math.min(x, y ) + Math.abs(x - y);
        var rD2 = Math.min(numberOfCells - x - 1, y ) + Math.abs( numberOfCells - x - 1 - y);
        var rD3 = Math.min(numberOfCells - x - 1, numberOfCells - y - 1 ) + Math.abs( y - x );
        var rD4 = Math.min(numberOfCells - y - 1, x ) + Math.abs( numberOfCells - y - 1 - x);
        var rLeftUpFrac = (1.0 -  rD1 / ( numberOfCells - 1.0));
        var rRightUpFrac = ( 1.0 -  rD2 / ( numberOfCells - 1.0)) ;
        var rRightDownFrac =  ( 1.0 -  rD3 / ( numberOfCells - 1.0)) ;
        var rLeftDownFrac = ( 1.0 -  rD4 / ( numberOfCells - 1.0)) ;


        var iC1 = lefttopColor.b
        var iC2 = righttopColor.b
        var iC3 = rightbottomColor.b
        var iC4 = leftbottomColor.b
        var iBlue = iC1 * rLeftUpFrac + iC2 * rRightUpFrac + iC3 * rRightDownFrac + iC4 * rLeftDownFrac;

        iC1 = lefttopColor.g
        iC2 = righttopColor.g
        iC3 = rightbottomColor.g
        iC4 = leftbottomColor.g
        var iGreen = iC1 * rLeftUpFrac + iC2 * rRightUpFrac + iC3 * rRightDownFrac + iC4 * rLeftDownFrac;

        iC1 = lefttopColor.r
        iC2 = righttopColor.r
        iC3 = rightbottomColor.r
        iC4 = leftbottomColor.r
        var iRed = iC1 * rLeftUpFrac + iC2 * rRightUpFrac + iC3 * rRightDownFrac + iC4 * rLeftDownFrac;

        return Qt.rgba(iRed,iGreen,iBlue,1)
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
        PropertyChanges { target: dropDown; height:160 }
    }

    transitions: Transition {
        NumberAnimation { target: dropDown; properties: "height"; easing.type: Easing.OutExpo; duration: 400 }
    }
}


