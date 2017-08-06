import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick 2.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Item {
    width: 140
    x : 5
    y : 5
    height: parent.height -10
    property var currentColumn

     function setOperations(operations){
        operationlist.model = operations
     }

    function setOperation(newindex){
        operationlist.currentIndex = newindex
         columnOperation.columnIndex = currentColumn ? currentColumn.columnIndex : 0
        if ( operationlist.model && newindex < operationlist.model.length ){
            columnOperation.currentOperation = operationlist.model[newindex]
        }
    }

    onCurrentColumnChanged: {
        if ( currentColumn)
            operationlist.model = currentColumn.operations
        else{
           // operationlist.model = null
          //  columnOperation.currentOperation = null
        }
    }

    Rectangle {
        id : title
        width : parent.width - 2
        height : 18
        color : uicontext.paleColor
        Text {
            text : qsTr("Operations")
            width : parent.width
            font.bold: true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip {
            target : title
            text : qsTr("All operations that are applicable to the selected column")
        }
    }

    ListView {
        id : operationlist
        width : parent.width - 3
        height : parent.height - title.height - 5
        anchors.top : title.bottom
        anchors.topMargin: 4
        x : 3
        Component {
            id: operationHighlight

            Rectangle {
                width: operationlist.width - 6; height: 14
                x : 3
                color: Global.selectedColor; radius: 2
                y: (operationlist && operationlist.currentItem) ? operationlist.currentItem.y : 0
            }
        }
        highlight: operationHighlight
        delegate :  Component {
            Loader {
                sourceComponent: Component {
                    Text {
                        x : 4
                        text: name
                        width : operationlist.width
                        height : 14
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                setOperation(index)
                            }
                        }
                    }
                }
            }
        }

    }
}

