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

WorkBenchShifter {
    clip : true
    state : "invisible"

    property WorkSpaceModel workspace : uicontext.currentWorkSpace

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

       Component.onCompleted: {
//          state : "visible"
//          opacity : 1
           workspaceinfo.state = workspace && !workspace.isDefault ? "fullsize" : "zerosize"
      }
}

