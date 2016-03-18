import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0

import "../../Global.js" as Global

Rectangle {
    width : parent.width
    anchors.margins: 2
    color : tabmodel && tabmodel.side == "right" ? Global.alternatecolor3 : "white"
    ScrollView {

        anchors.fill : parent
        anchors.margins: 2
        GridView {
            id: thumbGrid
            property var oldLoader
            property var oldImageName
            model: currentCatalog ? currentCatalog.coverages : null
            delegate: DCatalogListThumb{}
            cacheBuffer: 1000
            cellWidth: 350
            cellHeight: 170
            anchors.fill: parent
            clip : true
            focus : true
        }

    }
}
