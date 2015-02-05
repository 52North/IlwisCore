import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../controls" as Controls
import "../Global.js" as Global
import QtQuick 2.0

Item {
    id : container
    clip : true
    opacity : 0

    signal unloadcontent(string content)

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/workspaceCS1.png"
        headerText:qsTr("Workspace builder")
    }

    Column {
        id : mainItems
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        height: parent.height - 100
        x : functionBarHeader.x

        Row {
            width : parent.width - 20
            height : Global.rowHeight
            x : 5
            Text{
                id : label1
                text : qsTr("Workspace name")
                width : 100
            }

            TextField{
                height : parent.height
                width : parent.width - label1.width
            }
        }

        Text {
            text : qsTr("Data")
            height: Global.rowHeight
            width : parent.width
            font.bold: true
        }

        Rectangle {
            width : parent.width
            height : parent.height / 4
            color : Global.alternatecolor4
            GridView {
                anchors.fill: parent
                id : data

            }
        }

        Text {
            text : qsTr("Operations")
            height: Global.rowHeight
            width : parent.width
            font.bold: true
        }

        Rectangle {
            width : parent.width
            height : parent.height / 4
            color : Global.alternatecolor4
            GridView {
                anchors.fill: parent
                id : operations

            }
        }

        Text {
            text : qsTr("Description")
            height: Global.rowHeight
            width : parent.width
            font.bold: true
        }
        Rectangle {
            width : parent.width
            height : parent.height / 5
            border.width: 1
            border.color: Global.alternatecolor1
            TextArea {
                anchors.fill: parent
                id : description

            }
        }

    }
    Action {
        id :addbookmark
        onTriggered: {
            //mastercatalog.addBookmark(currentFolder)
            //bookmarkadded()
        }
    }

    Controls.ActionButton{
        id : addBookmarkButton
        anchors.right : parent.right
        anchors.top: mainItems.bottom
        anchors.rightMargin: 5
        anchors.topMargin: 5
        buttontext : "Add\nBookmark"
        iconsource : "../images/addbookmarkCS1.png"
        //height : 0
        width :95
        action : addbookmark
        z : 1
    }

    states: [
        State { name: "visible"

            PropertyChanges {
                target: container
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: container
                opacity : 0
            }
        }

    ]
      transitions: [
          Transition {
              NumberAnimation {
                  properties: "opacity"; duration : 500 ; easing.type: Easing.Linear
              }
              onRunningChanged :
              {
                  if ( opacity == 0) {
                    unloadcontent("Navigator.qml")
                  }
              }

          }
      ]

      Component.onCompleted: {
          state : "visible"
          opacity : 1
      }
}

