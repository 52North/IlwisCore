import QtQuick 2.1
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    id : displayEditorColumn

    property var currentEditor
    color : Global.alternatecolor3
    onCurrentEditorChanged: {
         if ( currentEditor){
             if ( editorColumn2.state == "minimized"){
                 editorColumn1.state = "minimized"
                 editorColumn2.state = "maximized"
                 propertyEditor2.setSource(currentEditor.qmlUrl,{"editor" : currentEditor})

             }else {
                 editorColumn2.state = "minimized"
                 editorColumn1.state = "maximized"
                 propertyEditor1.setSource(currentEditor.qmlUrl,{"editor" : currentEditor})
             }
          }
     }
    Rectangle {
        id : editorsLabel
        width : parent.width
        height : 18
        color : uicontext.paleColor
        Text{
            text : qsTr("Property Editor")
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Controls.ToolTip{
            target: editorsLabel
            text : qsTr("Editor for the a set of visual properties of a data attribute")
        }
    }

    Rectangle {
        id : editorColumn1
        color : "transparent"
        border.color: "lightgrey"
        border.width: 1
        anchors.right: parent.right
        anchors.top: editorsLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 3
        state : "maximized"
        Loader {
            id : propertyEditor1
            width : parent.width
            height : parent.height


        }
        states: [
            State { name: "maximized"
                    PropertyChanges { target: editorColumn1; opacity : 1 }
                    PropertyChanges { target: editorColumn1; enabled : true }
            },
            State {
                name : "minimized"
                    PropertyChanges { target: editorColumn1; opacity : 0 }
                    PropertyChanges { target: editorColumn1; enabled : false }
            }

        ]
        transitions: [
            Transition {
                NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
            }
        ]

    }

    Rectangle {
        id : editorColumn2
        color : "transparent"
        anchors.right: parent.right
        anchors.top: editorsLabel.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 3
        Loader {
            id : propertyEditor2
            width : parent.width
            height : parent.height


        }
        states: [
            State { name: "maximized"
                    PropertyChanges { target: editorColumn2; opacity : 1 }
                    PropertyChanges { target: editorColumn2; enabled : true }
            },
            State {
                name : "minimized"
                    PropertyChanges { target: editorColumn2; opacity : 0 }
                    PropertyChanges { target: editorColumn2; enabled : false }
            }

        ]
        transitions: [
            Transition {
                NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
            }
        ]

    }
}

