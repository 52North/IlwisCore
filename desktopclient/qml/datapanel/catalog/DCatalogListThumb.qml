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
import "../../controls" as Controls

Item{
    id: thumbDelegate
    width :  GridView.view.cellWidth
    height :  GridView.view.cellHeight
    clip : true

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
        height : parent.height
        width : 150
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

    }
    Image {
        id : typeicon
        y : 8
        anchors.left : page.right
        anchors.leftMargin: 5
        source : iconSource(iconPath)
        width : 20
        height : 20

    }
    Text{
        id: label
        text: displayName
        y : 10
        height : 13
        width : parent.width - 15
        elide: Text.ElideMiddle
        horizontalAlignment: Text.AlignLeft
        font.family: "Verdana"
        font.bold: true
        font.pointSize: 10
        anchors.left : typeicon.right
        anchors.leftMargin: 2
    }
    Column {
        anchors.top : label.bottom
        anchors.topMargin: 16
        width : parent.width
        height : thumbDelegate.height - label.height - 4
        anchors.left : page.right
        anchors.leftMargin: 5

        Controls.TextEditLabelPair{
            width : 240
            content : dimensions
            labelWidth: 80
            readOnly: true
            labelText: qsTr("Dimensions")
            height : 17
            boldLabel: false
        }
        Controls.TextEditLabelPair{
            width : 240
            content : domainType
            labelWidth: 80
            readOnly: true
            labelText: qsTr("Domain type")
            height : 17
            boldLabel: false
        }
        Controls.TextEditLabelPair{
            width : 240
            content : domainName
            labelWidth: 80
            readOnly: true
            labelText: qsTr("Domain")
            height : 17
            boldLabel: false
        }
        Controls.TextEditLabelPair{
            width : 240
            content : coordinateSystemName
            labelWidth: 80
            readOnly: true
            labelText: qsTr("Coord system")
            height : 17
            boldLabel: false
        }
        Controls.TextEditLabelPair{
            width : 240
            content : typeName == "rastercoverage" ? geoReferenceName : ""
            labelWidth: 80
            readOnly: true
            labelText: typeName == "rastercoverage" ? qsTr("Georeference") : ""
            height : 17
            boldLabel: false
        }
        Controls.TextEditLabelPair{
            width : 240
            content : makeSize(size)
            labelWidth: 80
            readOnly: true
            labelText: qsTr("Physical size")
            height : 17
            boldLabel: false
        }

    }

    MouseArea{
        id : mouseArea
        anchors.fill: parent
        anchors.topMargin: 40
        property variant image
        drag.target: image
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            thumbGrid.currentIndex = index;
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
            }', mouseArea, "dynamicImage")
        }
    }

}

