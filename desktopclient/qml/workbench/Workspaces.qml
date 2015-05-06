import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import WorkSpaceModel 1.0
import "../controls" as Controls
import "../Global.js" as Global
import "./workspace" as WorkSpace
import QtQuick 2.0

Item {
    id : container
    clip : true
    opacity : 0

    signal unloadcontent(string content)
    property WorkSpaceModel workspace : mastercatalog.currentWorkSpace

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/workspaceCS1.png"
        headerText:qsTr("Workspace builder")
    }

    Item {
        id : mainItems
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        height : parent.height - 40
        x : functionBarHeader.x

        WorkSpace.WorkSpaceCreation{
            id : creation

        }
        WorkSpace.WorkSpaceInfo{
            id : workspaceinfo
            anchors.top: creation.bottom
        }
    }
    Action {
        id :addbookmark
        onTriggered: {
            if ( workspace)
                mastercatalog.addBookmark(workspace.url)
        }
    }

    Controls.ActionButton{
        id : addBookmarkButton
        anchors.right : parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 5
        anchors.bottomMargin: 10
        buttontext : "Add\nBookmark"
        iconsource : "../images/addbookmarkCS1.png"
        height : 40
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
          workspaceinfo.state = workspace ? "fullsize" : "zerosize"
      }
}

