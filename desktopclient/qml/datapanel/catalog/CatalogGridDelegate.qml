import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0

import "../../Global.js" as Global

Item {
    id: iconDelegate
    x: 2 * Global.uiScale
    y : 3 * Global.uiScale
    width :  GridView.view.cellWidth
    height :  GridView.view.cellHeight


    function iconSource(name) {
        if ( name === "")
            name = "redbuttonr.png"
         var iconP = "../../images/" + name
         return iconP

     }

    Row {
        anchors.fill: parent
        spacing : 6 * Global.uiScale
        Image{
            id : imageBase
            x : 0
            y : 0
            width :18 * Global.uiScale * 8.5/9.0; height : parent.height * Global.uiScale * 8.5/9.0
            source : iconSource(iconPath)
            fillMode: Image.PreserveAspectFit
        }

        Rectangle{
            width : 130 * Global.uiScale
            height : iconDelegate.height
            color : isSelected ? Global.selectedColor : "transparent"
            anchors.verticalCenter: imageBase.verticalCenter
            Text {

                id: txt
                text: displayName
                elide: Text.ElideMiddle
                color: itemgrid.currentIndex === index ? "blue": "black"
                width : parent.width
                height : parent.height * Global.uiScale * 8.5/9.0

                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter

                font.pointSize: 8 * Global.uiScale
               // verticalAlignment: Text.AlignVCenter

            }
        }
    }
    MouseArea{
        anchors.fill: parent
        id : mouseArea
        property variant image
        drag.target: image
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            itemgrid.currentIndex = index;
            isSelected = !isSelected
            catalogViews.setSelected(id)
            if (catalogViews && catalogViews.tabmodel && !catalogViews.tabmodel.selected)
                catalogViews.tabmodel.selectTab()
        }

        onDoubleClicked: {
            if ( name == "..")
                showObject(-1)
            else {
                showObject(id)
                isSelected = true
                catalogViews.setSelected(id)
            }
        }
        onReleased: {
            image.Drag.drop()
            image.parent = mouseArea
            image.anchors.fill = mouseArea
            image.destroy();
        }
        onPressed: {
           image = Qt.createQmlObject('import QtQuick 2.0; Image{
                id : image
                width : 20; height : 20
                source : iconSource(iconPath)
                fillMode: Image.PreserveAspectFit
                property string message :  model !== null ? url : ""
                property string ilwisobjectid : model !== null ? id : ""
                property string type : model !== null ? typeName : ""
                property string ids : model !== null ? mastercatalog.selectedIds() : ""

                Drag.keys: typeName
                Drag.active: mouseArea.drag.active
                Drag.hotSpot.x: 10
                Drag.hotSpot.y: 10
                opacity : Drag.active / 2

                states: State {
                    when: mouseArea.drag.active
                    ParentChange { target: image; parent: root }
                    AnchorChanges { target: image; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
                }
            }', mouseArea, "dynamicImage");
          }
    }

}

