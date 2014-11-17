import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0

Rectangle {
    anchors.fill :parent
    color : "white"

    Image {
        anchors.fill: parent
        source : "../../images/remoteBig.png"
        fillMode: Image.Stretch
        opacity: 0.07

    }

    ScrollView {
        id : catalogGrid
        property color background1 : "#EAECEE"
        property color background2 : "#FFF9E3"

        anchors.fill : parent
        anchors.margins: 2

        GridView {
            id : itemgrid


            model : mastercatalog.resources
            delegate: CatalogGridDelegate{}
            cellWidth: 170
            cellHeight: 25
            clip : true
            highlight: Rectangle {
                color: "#448"
            }
            focus : true
            cacheBuffer: 1000
            highlightFollowsCurrentItem: true




        }
    }
}



