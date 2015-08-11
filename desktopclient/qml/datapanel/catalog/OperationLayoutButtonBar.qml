import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import CatalogModel 1.0
import MasterCatalogModel 1.0
import CatalogFilterModel 1.0
import TabModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    property int heightButtons :22
    Row {
        id : layoutfilter
        width : 95
        height : 22

        ExclusiveGroup { id : operationViewStatus
            onCurrentChanged: {
                if( showList.checked)
                    catalogView.state = "iconList"
                if ( showGrid.checked)
                    catalogView.state = "iconGrid"
            }
        }
        ToolBarButton{
            id : showList
            iconSource: iconsource("listCS1.png")
            exclusiveGroup: operationViewStatus
            checked : false
        }
        ToolBarButton{
            id : showGrid
            iconSource: iconsource("gridCS1.png")
            checked : true
            exclusiveGroup: operationViewStatus
        }
    }
    //        Button{
    //            id : metadata
    //            implicitHeight: heightButtons
    //            width : heightButtons + 4
    //            iconSource: iconsource("metadata20.png")
    //            anchors.left : refresh.right
    //            anchors.leftMargin: 1
    //            anchors.top: parent.top
    //            anchors.topMargin: 2
    //            checkable: false
    //            action : showProps
    //        }


}


