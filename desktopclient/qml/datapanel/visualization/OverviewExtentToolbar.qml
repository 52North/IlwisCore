import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../workbench/propertyform" as MetaData
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Rectangle {
    height : parent.height
    width : 30
    color : Global.alternatecolor1
    Column{
        spacing : 2
        anchors.horizontalCenter: parent.horizontalCenter
        width : parent.width
        height : parent.height
        MapExtentButton{
            id : panButton1
            icon : "pan20.png"
        }

        MapExtentButton{
            id : zoominButton1
            icon : zoominButton1.checked ? "zoomin20A.png" : "zoomin20.png"
           // action : zoomClicked
            checkable: true
            checked: false
            onClicked: {
                checked = !checked
            }
        }
        MapExtentButton{
            id : zoomoutButton1
            icon :"zoomout20.png"
        }
    }
}
