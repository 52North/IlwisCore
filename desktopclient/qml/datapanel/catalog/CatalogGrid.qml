import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0
import UIContextModel 1.0

import "../../Global.js" as Global

FocusScope {
    anchors.fill: parent
    property color backgroundColor: "white"
    Rectangle {
        anchors.fill :parent
        color : "white"

        ScrollView {
            id : catalogGrid
            anchors.fill : parent
            anchors.margins: 2

            GridView {
                id : itemgrid

                Loader {
                    id : floatingProps
                    width :270
                    height : 150
                    z : 100
                }

                model : setResources()
                width : parent.width * Global.uiScale
                height : parent.height * Global.uiScale

                delegate: CatalogGridDelegate{}
                cellWidth: 170 * Global.uiScale
                cellHeight: 22 * Global.uiScale
                clip : true
                cacheBuffer: 1000
                flow: GridView.FlowTopToBottom
            }
        }
    }
}



