import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0

import "../../Global.js" as Global
import "../../controls" as Controls

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
    Controls.ToolTip{
        id : tooltip
        target : iconDelegate
        text : typeName + (description !== "" ? "\n"+ description : "")
    }

    Row {
        anchors.fill: parent
        spacing : 6
        Image{
            id : imageBase
            x : 0
            y : 0
            width : 21; height : 21
            source : iconSource(iconPath)
            fillMode: Image.PreserveAspectFit
        }

        Rectangle{

            width : 130
            height : iconDelegate.height
            color : isSelected ? Global.selectedColor : "transparent"
            anchors.verticalCenter: imageBase.verticalCenter



            Text {
                function getText(){
//                    if ( extendedType){
//                        return "~" + displayName
//                    }
                    return displayName
                }
                id: txt
                text: txt.getText()
                elide: Text.ElideMiddle
                color: itemgrid.currentIndex === index ? "blue": "black"
                width : parent.width
                height : parent.height * 8.5/9.0

                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter

                font.pointSize: 8
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
        hoverEnabled: true

        onPositionChanged: {
            tooltip.stopTimer()
            if ( floatingProps.item){
                floatingProps.item.setObject(id)
                var point = floatingProps.mapFromItem(mouseArea, mouseX, mouseY)
                floatingProps.item.x = point.x
                floatingProps.item.y =point.y
            }
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
            if ( image){
                image.Drag.drop()
                image.parent = mouseArea
                image.anchors.fill = mouseArea
                image.destroy();
            }
//            if ( floatingProps && floatingProps.item){
//                floatingProps.source=""
//            }
        }
        onPressed: {
            itemgrid.currentIndex = index;
            isSelected = !isSelected
            catalogViews.setSelected(id)
            if (catalogViews && catalogViews.tabmodel && !catalogViews.tabmodel.selected)
                catalogViews.tabmodel.selectTab()
            var key = uicontext.currentKey
 //           if ( key !== Qt.Key_Alt){
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
//            }else {
//                floatingProps.source = "QuickProps.qml"
//                floatingProps.item.setObject(id)
//                var point = floatingProps.mapFromItem(mouseArea, mouseX, mouseY)
//                floatingProps.item.x = point.x
//                floatingProps.item.y =point.y
//            }
        }
    }

}

