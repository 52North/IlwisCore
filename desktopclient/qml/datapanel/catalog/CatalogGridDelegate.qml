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
    x: 2
    y : 3
    width :  GridView.view.cellWidth
    height :  GridView.view.cellHeight

    function iconSource(name) {
        if ( name === "")
            name = "redbuttonr.png"
         var iconP = "../../images/" + name
         return iconP

     }


    MouseArea {
        id : mouseArea
        property variant image
        width : 20; height : parent.height
        drag.target: image
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
        z : 1
    }

    Image{
        id : imageBase
        x : mouseArea.x
        y : mouseArea.y
        width :18; height : parent.height - 2
        source : iconSource(iconPath)
        fillMode: Image.PreserveAspectFit
        z : 0
    }

    Rectangle{
        width : 130
        height : iconDelegate.height
        color : isSelected ? Global.selectedColor : "transparent"
        anchors.left : mouseArea.right
        anchors.leftMargin: 6
        z : 0

        Text {
            id: txt
            text: displayName
            elide: Text.ElideMiddle
            color: itemgrid.currentIndex === index ? "blue": "black"
            width : parent.width

            y : 4
            font.pointSize: 8
            verticalAlignment: Text.AlignVCenter
            z : 1

        }
    }
    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            itemgrid.currentIndex = index;
            isSelected = !isSelected
            setSelected(id)
            if (catalogViews && !catalogViews.tabmodel.selected)
                catalogViews.tabmodel.selectTab()
        }

        onDoubleClicked: {
            if ( name == "..")
                showObject(-1)
            else {
                showObject(id)
                isSelected = true
                setSelected(id)
            }
        }
    }

}

