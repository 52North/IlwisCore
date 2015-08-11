import QtQuick 2.2
import CatalogModel 1.0

Rectangle {
    id : catalogProps
    property CatalogModel catalogmd
    anchors.fill: parent

    ListView {
        id : objectlist
        anchors.fill: parent
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
        if ( catalogmd){
            objectlist.model = catalogmd.objectCounts()
        }
    }
}

