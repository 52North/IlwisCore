import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0

ToolBar{
    id : workflowtools
    width : parent.width
    height : 31
    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            return ""

        var iconP = "../../../qml/images/" + name
        return iconP
    }

    Button {
        height : 25
        width : 25
        Image {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            source : iconsource("data_folder.png")
        }
        onClicked: {
            var url = mastercatalog.currentCatalog.url
            console.debug("c", url)
            if ( url.indexOf("file://") !== 0) {
                return
            }
            modellerDataPane.model.store(url)
        }
    }
}
