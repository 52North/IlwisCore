import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import CatalogModel 1.0
import MasterCatalogModel 1.0
import ".." as Base
import "../../Global.js" as Global

Rectangle {
    id : catalogProps
    property CatalogModel catalogmd
    anchors.fill: parent
    color : uicontext.lightestColor

    Button{
        id : scanButton
        width : 100
        height : 20
        text : qsTr("Rescan")
        onClicked: {
            catalogmd.scanContainer();
        }

    }

    ListView {
        id : objectlist
        height :parent.height - scanButton.height - 5
        width : parent.width
        anchors.bottom: parent.bottom
        model : catalogmd ? catalogmd.objectCounts : null


        delegate: Item {
            width : parent.width
            height : 15
            Row {
                width : parent.width
                height : 15
                Text {
                    width : 170
                    height : 15
                    text : modelData.split("|")[0]
                }
                Text {
                    height : 15
                    text : modelData.split("|")[1]
                }
            }
        }

    }

    Component.onCompleted: {
        catalogmd = catalog(id)
        //model = catalogmd.objectCounts
    }
}

