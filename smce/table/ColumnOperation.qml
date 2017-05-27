import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../../../../qml" as Base

Item {
    id : displayEditorColumn

    property var currentOperation
    property int columnIndex

    onCurrentOperationChanged: {
         if ( currentOperation){
             if ( operationColumn2.state == "minimized"){
                 operationColumn1.state = "minimized"
                 operationColumn2.state = "maximized"
                 operation2.setSource(currentOperation.qmlUrl,{"operation" : currentOperation, "columnIndex" : columnIndex})

             }else {
                 operationColumn2.state = "minimized"
                 operationColumn1.state = "maximized"
                 operation1.setSource(currentOperation.qmlUrl,{"operation" : currentOperation, "columnIndex" : columnIndex})
             }
          }else {
             operation1.source = ""
             operation2.source = ""
         }
     }

    Rectangle {
        id : operationLabel
        width : parent.width - 2
        height : 18
        color : uicontext.paleColor
        Text {
            text : qsTr("Operation")
            width : parent.width
            font.bold: true
            x : 5
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip {
            target : operationLabel
            text : qsTr("Operation that changes the content/organization of a column or adds a new column")
        }
    }

    Rectangle {
        id : operationColumn1
        color : uicontext.workbenchBGColor
        border.color: "lightgrey"
        border.width: 1
        anchors.right: parent.right
        anchors.top: operationLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 3
        state : "maximized"
        Loader {
            id : operation1
            width : 160
            height : parent.height


        }
        states: [
            State { name: "maximized"
                    PropertyChanges { target: operationColumn1; opacity : 1 }
                    PropertyChanges { target: operationColumn1; enabled : true }
            },
            State {
                name : "minimized"
                    PropertyChanges { target: operationColumn1; opacity : 0 }
                    PropertyChanges { target: operationColumn1; enabled : false }
            }

        ]
        transitions: [
            Transition {
                NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
            }
        ]

    }

    Rectangle {
        id : operationColumn2
        color : uicontext.workbenchBGColor
        border.color: "lightgrey"
        border.width: 1
        anchors.right: parent.right
        anchors.top: operationLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 3
        Loader {
            id : operation2
            width : 160
            height : parent.height


        }
        states: [
            State { name: "maximized"
                    PropertyChanges { target: operationColumn2; opacity : 1 }
                    PropertyChanges { target: operationColumn2; enabled : true }
            },
            State {
                name : "minimized"
                    PropertyChanges { target: operationColumn2; opacity : 0 }
                    PropertyChanges { target: operationColumn2; enabled : false }
            }

        ]
        transitions: [
            Transition {
                NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
            }
        ]

    }
}



