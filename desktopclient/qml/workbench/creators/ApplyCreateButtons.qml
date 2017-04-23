import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Item {
    property var createObject

    function currentCatalogCorrectUrl(){ // must be a file location
        var panel = datapanel.activeItem
        if ( !panel)
            return ""
        if ( panel.panelType === "catalog"){
            var url = panel.currentCatalog.isFileBased ? panel.currentCatalog.container : panel.currentCatalog.url
            if ( url.indexOf("file://") !== 0) {
                overwrite.visible = false
                return ""
            }
            return url
        }
        overwrite.visible = false
        return ""
    }

    Button {
        id : applybutton
        anchors.right: parent.right
        width : 70
        text : qsTr("Apply")
        y : 10
        enabled: currentCatalogCorrectUrl() !== ""
        onClicked: {
            editorList.pop()
            if (!createObject(false)){
                overwrite.visible = true
            }
        }

    }
    Button {
        id : closebutton
        anchors.right: applybutton.left
        anchors.rightMargin: 5
        width : 70
        text : qsTr("Close")
        y : 10

        onClicked: {
            dropItem.state = "invisible"
        }
    }
    Text {
        id : wronglocation
        width :150
        height : parent.height
        text : qsTr("Location can't be used for writing")
        anchors.right: closebutton.left
        anchors.rightMargin: 5
        visible : currentCatalogCorrectUrl() === "" ? true : false
        color : "red"

    }
    Rectangle {
        id : overwrite
        anchors.right: closebutton.left
        anchors.rightMargin: 5
        visible : false
        enabled : visible
        width : 185
        height : 45
        color : "#ffe6cc"
        Text{
            y : 15
            id : message
            width : 105
            height : 20
            text : qsTr("Object already exists")
            anchors.right: parent.right

        }
        Button {
            anchors.right: message.left
            anchors.rightMargin: 5
            width : 70
            text : qsTr("Overwrite")
            y : 10

            onClicked: {
                createObject(true)
                overwrite.visible = false
            }
        }

    }
}
