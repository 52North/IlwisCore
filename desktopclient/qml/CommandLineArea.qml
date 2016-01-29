import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import MasterCatalogModel 1.0
import "Global.js" as Global
import "controls" as Controls

Rectangle {
    id : commLine
    height : textArea.height
    color : Global.alternatecolor5
    Row {
        id : workspace
        height : parent.height
        width : 250
        spacing: 10
        x : 5
        Text {
            y : 6
            text : qsTr("Current Workspace")
        }
        ComboBox{
            width : 150
            height : 25
            objectName: "workspace_combobox_mainui"
            model : mastercatalog.workspaces
            textRole: "displayName"
            y : 2
            onCurrentIndexChanged: {
                if ( currentIndex >= 0){
                    var wmodel =model[currentIndex]
                    if ( wmodel){
                        var filter = "resource='" + wmodel.url + "'"
                        mainSplit.changeCatalog(filter,"catalog",wmodel.url)
                        uicontext.currentWorkSpace = wmodel
                    }
                }
            }

        }
    }
    Control.CommandLine{
        id : textArea
        y : 2
        anchors.left : workspace.right
        anchors.leftMargin: 2
        anchors.right: parent.right
    }
}

