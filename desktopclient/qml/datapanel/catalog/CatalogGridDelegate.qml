import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0

Item {
    id: iconDelegate
    x: 2
    y : 3
    width :  GridView.view.cellWidth
    height :  GridView.view.cellHeight

    function iconSource(name) {
         var iconP = "../../images/" + name
         return iconP

     }

    MouseArea {
        id : mouseArea
//        x : 2
//        y : 2
        width : 20; height : 20
        drag.target: image
        onReleased: {
            image.Drag.drop()
            image.parent = mouseArea
            image.anchors.fill = mouseArea
        }

        Image{
            id : image
            width : 20; height : 20
            source : iconSource(iconPath)
            fillMode: Image.PreserveAspectFit
            property string message :  url

            Drag.keys: [ iconPath ]
            Drag.active: mouseArea.drag.active
            Drag.hotSpot.x: 10
            Drag.hotSpot.y: 10
            opacity : Drag.active / 2


            states: State {
                when: mouseArea.drag.active
                ParentChange { target: image; parent: root }
                AnchorChanges { target: image; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
            }
        }
        z : 1
    }
    Image{
        id : imageBase
        x : mouseArea.x
        y : mouseArea.y
        width : 20; height : 20
        source : iconSource(iconPath)
        fillMode: Image.PreserveAspectFit
        z : 0
    }

    Text {
        id: txt
        text: displayName
        elide: Text.ElideMiddle
        color: itemgrid.currentIndex === index ? "white": "black"
        anchors.left : mouseArea.right
        anchors.leftMargin: 6
        width : 130
        y : 4
        verticalAlignment: Text.AlignVCenter
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {
            itemgrid.currentIndex = index;
         }
        onDoubleClicked: {
             showObject(id)
        }
    }

}

