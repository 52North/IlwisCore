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
    Rectangle {
        anchors.fill :parent
        color : tabmodel && tabmodel.side == "right" ? Global.alternatecolor4 : "white"

        ScrollView {
            id : catalogGrid
            property color background1 : "#EAECEE"
            property color background2 : "#FFF9E3"

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
                cellHeight: 18 * Global.uiScale
                clip : true
                cacheBuffer: 1000
                flow: GridView.FlowTopToBottom
            }
        }
    }
}



