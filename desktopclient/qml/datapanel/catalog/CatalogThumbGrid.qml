import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0

Item {
    width : parent.width
    anchors.margins: 2
    GridView {
        id: thumbGridView
        model: mastercatalog.resources
        delegate: DCatalogThumb{}
        cacheBuffer: 1000
        cellWidth: 150
        cellHeight: 230
        anchors.fill: parent
        clip : true
        highlight: Rectangle {
            color: "#6084c2"
        }
        focus : true
    }


}
