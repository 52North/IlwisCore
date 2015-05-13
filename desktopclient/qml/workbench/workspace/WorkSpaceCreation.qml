import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import WorkSpaceModel 1.0
import "../../Global.js" as Global

Row {
    width : parent.width - 20
    height : Global.rowHeight
    x : 5
    Text{
        id : label1
        text : qsTr("Workspace name")
        width : 100
    }

    ComboBox{
        id : names
        height : parent.height
        width : parent.width - label1.width - 55
        model : mastercatalog.workspaces
        textRole: "displayName"
        editable : true
        onEditTextChanged: {
            if ( editText != "")
                createButton.enabled = true
        }
        onCurrentIndexChanged: {
            if ( names.model && currentIndex >= 0 && currentIndex < names.model.length)
                workspace = names.model[currentIndex]
                if ( currentIndex > 0) // we dont do anything on 0 as that is default workspace
                    workspaceinfo.state = "fullsize"
                else
                    workspaceinfo.state = "zerosize"
        }

    }
    Button {
        id : createButton
        text : "create"
        width : 50
        height : 20
        enabled : false
        onClicked: {
            mastercatalog.addWorkSpace(names.editText)
            workspace = mastercatalog.workspace(names.editText)
            if ( workspace){
                data.model = workspace.data
                workspaceinfo.state = "fullsize"
            }
        }
    }
}


