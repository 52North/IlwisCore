import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0
import LayersView 1.0
import "../../Global.js" as Global

Item{
id: thumbDelegate
    width :  GridView.view.cellWidth
    height :  GridView.view.cellHeight

    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( !name || name === "?")
            name = "redbuttonr.png"

         var iconP = "../../images/" + name
         return iconP

     }

    function makeSize(size){
        if ( size < 1000)
            return size
        if ( size >= 1000 && size < 1e6){
            return size.toString() + "( " + (size/1000).toFixed(2).toString() + " Kb )"
        }
        if ( size >= 1e6 && size < 1e9){
            return size.toString() + "( " + (size/1e6).toFixed(2).toString() + " Mb )"
        }
        if ( size >= 1e9 ){
            return size.toString() + "( " + (size/1e9).toFixed(2).toString() + " Gb )"
        }

    }
    Component {
        id : defaultImage
        Image {
            id: img
            anchors.fill: parent
            anchors.margins: 2

            source: iconSource(imagePath)
            fillMode: Image.PreserveAspectFit
            asynchronous: true
        }
    }

    Component {
        id : layerDrawer
        LayersView{
            id : lyrview
            anchors.fill: parent
            anchors.margins: 2
            objectName : "thumb_generator_mainui"
            Component.onCompleted: {
                var cmd = "adddrawer(" + viewerId + "," + url +",\"itemid=" + id + "\"," + typeName + ")"
                addCommand(cmd)
                setAttribute("GridDrawer", {"active" : false})
                realizeThumbPath()
                setAttribute("View",{"saveimage" : imagePath})
            }
        }
    }

    Image {
        id : page
        anchors.fill: parent
        source : iconSource("page.png")
        Rectangle {
            id : frame
            anchors.top : parent.top
            anchors.topMargin: 10
            anchors.left : parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            height : 150
            border .width: 1
            border.color: "lightgrey"
            Loader{
                id : imageLoader
                anchors.fill: parent
                sourceComponent : defaultImage
            }

        }

        Image {
            anchors.top : parent.top
            anchors.topMargin: 13
            anchors.left : parent.left
            anchors.leftMargin: 13
            source : iconSource(iconPath)
            width : 20
            height : 20

        }

        Button {
            id : refreshBut
            width : 16
            height : 18
            anchors.top : parent.top
            anchors.topMargin: 13
            anchors.right : parent.right
            anchors.rightMargin: 13
            Image {
                source : iconSource("refresh20.png")
                width : 14
                height : 16
                anchors.centerIn: parent
                MouseArea {
                    anchors.fill: parent
                    onClicked:{
                        imageLoader.sourceComponent = layerDrawer
                    }
                }
            }

        }
        Text{
            x: 14
            id: label
            text: displayName
            anchors.top : frame.bottom
            height : 13
            width : parent.width - 15
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            color: "black"
            font.family: "Verdana"
            font.pointSize: 7.5
        }
        Text{
            x: 14
            id: label2
            text: domainName
            anchors.top : label.bottom
            height : 13
            width : parent.width - 15
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            color: "black"
            font.family: "Verdana"
            font.pointSize: 7.5
        }
        Text{
            x: 14
            id: label3
            text: dimensions
            anchors.top : label2.bottom
            height : 13
            width : parent.width - 15
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            color: "black"
            font.family: "Verdana"
            font.pointSize: 7.5
        }
        Text{
            x: 14
            id: label4
            text: makeSize(size)
            anchors.top : label3.bottom
            height : 13
            width : parent.width - 15
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            color: "black"
            font.family: "Verdana"
            font.pointSize: 7.5
        }
    }
    MouseArea{
        id : mouseArea
        anchors.fill: parent
        property variant image
        drag.target: image
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        propagateComposedEvents: true
        onClicked: {
            thumbGrid.currentIndex = index;
            isSelected = !isSelected
            setSelected(id)
            mouse.accepted = false
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
            mouse.accepted = false
        }
        onReleased: {
            image.Drag.drop()
            image.parent = mouseArea
            image.anchors.fill = mouseArea
            image.destroy();
            mouse.accepted = false
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
            }', mouseArea, "dynamicImage")
            mouse.accepted = false
          }
    }

}

