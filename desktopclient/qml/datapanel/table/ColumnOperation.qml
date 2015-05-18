import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../.." as Base

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
          }
     }
    Rectangle {
        id : operationLabel
        width : parent.width
        height : 18
        color : Global.alternatecolor3
        Text{
            text : qsTr("Operation")
            font.weight: Font.DemiBold
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        id : operationColumn1
        color : Global.alternatecolor2
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
        color : Global.alternatecolor2
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



