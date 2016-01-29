import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

Row {
    width :87
    height : 22
    anchors.topMargin: 2
    anchors.leftMargin: 4

    ExclusiveGroup { id : catalogViewStatus
        onCurrentChanged: {
            if( showList.checked)
                catalogView.state = "bylocation"
            if ( showGrid.checked)
                catalogView.state = "iconGrid"
            if ( showThumbs.checked)
                catalogView.state = "thumbList"
        }
    }
    ToolBarButton{
        id : showList
        iconSource: iconsource("listCS1.png")
        exclusiveGroup: catalogViewStatus
        checked : false
    }
    ToolBarButton{
        id : showGrid
        iconSource: iconsource("gridCS1.png")
        checked : true
        exclusiveGroup: catalogViewStatus
    }
    ToolBarButton{
        id : showThumbs
        iconSource: iconsource("thumblistCS1.png")
        exclusiveGroup: catalogViewStatus
        checked: false
    }
}

