import QtQuick 2.1
import "../../Global.js" as Global

Rectangle{
    id : toolbar
    anchors.top : parent.top
    width : parent.width
    height : 52
    border.width: 1
    border.color: "grey"
    color : Global.alternatecolor5
    z : 1

    CatalogFilterButtons{
        id : objectfilters
    }

    CatalogFilterComboBox{
        id : catalogfilters
        anchors.top : objectfilters.bottom
        anchors.left : objectfilters.left
    }

    CatalogLayoutButtonBar {
        id : layoutfilter
        anchors.top : objectfilters.bottom
        anchors.left : catalogfilters.right
    }

}

