import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Item {
    id : inner
    width: 140
    x : 5
    y : 5
    height: parent.height -10
    property var columnmodel : table ? table.columns : null
    property alias currentIndex : columnlist.currentIndex

    function setCurrentIndex(newindex){
        if ( newindex){
            columnlist.currentIndex = newindex
            columnOperationList.currentColumn =columnlist.model[newindex]
            columnOperationList.setOperation(0)
        }else if ( newindex === 0){
            columnlist.currentIndex = newindex
            columnOperationList.currentColumn = null
            columnOperationList.setOperations(table.operations)
        }
    }

    Rectangle {
        id : title
        width : parent.width - 2
        height : 18
        color : uicontext.paleColor
        Text {
            text : qsTr("Columns")
            width : parent.width
            font.bold: true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip {
            target : title
            text : qsTr("All columns available in the table")
        }
    }

    ListView {
        id : columnlist
        width : parent.width - 3
        height :    parent.height - title.height
        anchors.top : title.bottom
        model : columnmodel
        onModelChanged: {
            columnOperationList.currentColumn = null
        }

        anchors.topMargin: 4
        x : 3
        clip : true
        highlight: attributeHighlight
        function iconsource(name) {
            if ( name.indexOf("/") !== -1)
                return name
            if ( name === "")
                name = "redbuttonr.png"

            var iconP = "../../images/" + name
            return iconP
        }

        Component {
            id: attributeHighlight

            Rectangle {
                width: columnlist.width - 6; height: 14
                color: Global.selectedColor; radius: 2
                y: (columnlist && columnlist.currentItem) ? columnlist.currentItem.y : 0
            }
        }
        delegate : Component {
            Row {
                width : 130
                height : 18
                spacing : 3
                Image{
                    id : domicon
                    source : columnlist.iconsource(icon)
                    width : 16
                    height : 16
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    text :  index == 0 ? qsTr("Table") : attributename
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            setCurrentIndex(index)
                        }
                    }
                }
            }
        }
    }
}

