import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayerManager 1.0
//import "./propertyeditors" as LayerManagement
import "../controls" as Controls
import "../Global.js" as Global

ToolBar{
    id : maptools
    width : parent.width
    height : 35
    Row {
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        spacing : 2
        MapExtentButton{
            icon  :"entiremap20.png"
            id : entireMap
            action : entireClicked
        }

        MapExtentButton{
            id : refreshButton
            icon : "refresh20.png"
            action : refreshClicked
        }

        MapExtentButton{
            id : panButton
            icon : "pan20.png"
        }

        MapExtentButton{
            id : zoominButton
            icon : zoominButton.checked ? "zoomin20A.png" : "zoomin20.png"
            action : zoomClicked
            checkable: true
            checked: false
            onClicked: {
                checked = !checked
            }
        }
        MapExtentButton{
            id : zoomoutButton
            icon :"zoomout20.png"
            action : zoomOutClicked
        }
    }
}


