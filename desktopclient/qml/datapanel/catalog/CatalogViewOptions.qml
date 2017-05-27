import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import LayersView 1.0
import QtGraphicalEffects 1.0
import UIContextModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls


Rectangle {
    anchors.fill: parent
    color : catalogSplit.backgroundCatalogColor
    property string iconName : "../images/selection"

    Component {
        id : layerDrawer
        LayersView{
            id : lyrview
            anchors.fill: parent
            property var resource
            property var controller
            function setResource(object, res){
                lyrview.associate(object.objectName,"drawEnded")
                var cmd = "adddrawer(" + viewerId + "," + res.url +",\"itemid=" + res.id + "\"," + res.typeName + ")"
                addCommand(cmd)
                setAttribute("GridDrawer", {"active" : false})
                res.realizeThumbPath()
                setAttribute("View",{"saveimage" : res.imagePath})
                lyrview.update()

            }

            anchors.margins: 2
        }
    }
    Rectangle {
        id : header
        width : leftContainer.width
        height : 18
        color : uicontext.paleColor
        Text{
            text : qsTr("Actions on Selected objects")
            font.bold: true
            x : 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        id : leftContainer
        width : 260
        height : parent.height - header.height
        anchors.top : header.bottom
        y : 4
        border.width: 1
        border.color : Global.edgecolor
        color :   uicontext.lightestColor


        Column {
            y: 5
            x : 5
            id : buttonRow
            width : parent.width
            height : parent.height - 5
            spacing : 4

            property int buttonWidth : 220

            Controls.WideButton{
                image : "../images/openmult20.png"
                label : qsTr("Open selected coverages\nin seperate panels")
                width : buttonRow.buttonWidth
                height : 40
                onClicked: {
                    var ids = mastercatalog.selectedIds()
                    var idlist = ids.split("|")
                    for(var i=0; i < idlist.length; ++i){
                        showObject(idlist[i]);
                    }
                }
            }

            Controls.WideButton{
                image : "../images/opensingle20.png"
                label : qsTr("Open selected coverages\nin new panel")
                width : buttonRow.buttonWidth
                height : 40
                onClicked: {
                    var ids = mastercatalog.selectedIds()
                    showObject(ids);
                }
            }
            Controls.WideButton{
                image : "../images/openfloat20.png"
                label : qsTr("Open selected coverages\nin floating panels")
                width : buttonRow.buttonWidth
                height : 40
                onClicked: {
                    var objectids = mastercatalog.selectedIds()
                    var qml = "import QtQuick 2.1; import QtQuick.Window 2.1;import \"..\" as FW;"
                    qml += "FW.FloatingWindow { id: floatingWindow } ";
                    var window = Qt.createQmlObject(qml, centerItem)

                    window.height = centerItem.activeItem.height
                    window.width = centerItem.activeItem.width
                    window.show();
                    var ids = objectids.split("|")
                    for ( var i=0; i < ids.length; ++i) {
                        var resource = mastercatalog.id2Resource(ids[i],0)
                        if ( resource){
                            if ( resource.typeName.indexOf("coverage") !== -1)
                                window.datapanel = "visualization/MapPanel.qml"
                            else if (resource.typeName === "table")
                                window.datapanel = "table/TablePane.qml"
                            else if (resource.typeName === "table")
                                window.datapanel = "table/TablePane.qml"
                            else if (resource.typeName === "workflow")
                                window.datapanel = "modeller/ModellerDataPane.qml"
                            window.addDataSource("itemid=" + ids[i],resource.url, resource.typeName)
                            resource.suicide()
                        }
                    }
                }
            }
            GroupBox {
                id : keyChanges
                title : qsTr("Keywords Selected Objects")
                height : Global.rowHeight * 4
                width : 220
                function changeKeywords(text){
                    if (text !== "")     {
                        var list = mastercatalog.selectedData
                        for(var i=0; i < list.length; ++i){
                            var resource = list[i]
                            if ( resource){
                                resource.keywords = keywords.text
                            }
                        }
                    }
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 3
                    spacing : 2
                    Row {
                        height : Global.rowHeight
                        width : buttonRow.buttonWidth - 20
                        TextField{
                            id : keywords
                            width :parent.width - 50
                            height : parent.height

                            Controls.ToolTip{
                                target: keywords
                                text : qsTr("Comma separated list of keywords will be added to the selected objects")
                            }
                        }
                        Button {
                            height : parent.height
                            width : 50
                            text : qsTr("Add")
                            onClicked:{
                                keyChanges.changeKeywords(keywords.text)
                            }
                        }
                    }
                    Row {
                        height : Global.rowHeight
                        width : buttonRow.buttonWidth - 20
                        TextField{
                            id : rem
                            width :parent.width - 50
                            height : parent.height

                            Controls.ToolTip{
                                target: keywords
                                text : qsTr("Remove a keyword from the selected objects")
                            }
                        }
                        Button {
                            height : parent.height
                            width : 50
                            text : qsTr("Remove")
                            onClicked:{
                                if (rem.text == "")
                                    keyChanges.changeKeywords("")
                                else {
                                    var list = mastercatalog.selectedData
                                    for(var i=0; i < list.length; ++i){
                                        var resource = list[i]
                                        if ( resource){
                                            var currentkeys = resource.keywords.split(',')
                                            var removekeys = rem.text.split(',')
                                            for(var j=0; j < removekeys.length; ++j){
                                                removekeys[j] = removekeys[j].trim()
                                            }
                                            var newkeys
                                            for(var current in currentkeys ){
                                                for(var deletekey in removekeys){
                                                    if ( current === deletekey){
                                                        current = ""
                                                        break;
                                                    }
                                                }
                                                if ( current !== ""){
                                                    if ( newkeys !== "")
                                                        newkeys += ","
                                                    newkeys += current
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Column {
        id : previews
        width : 240
        anchors.left: leftContainer.right
        anchors.leftMargin: 3
        height : parent.height - 5
        spacing : 6
        Rectangle {
            id : header2
            width : previews.width
            height : 18
            color : uicontext.paleColor
            Text{
                text : qsTr("Previews")
                font.bold: true
                x : 3
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Controls.WideButton{
            image : "../images/mapview.png"
            label : qsTr("Generate thumbs\nof selected coverages")
            width : parent.width - 3
            height : 40
            x : 8
            onClicked: {

                frame.makeResourceList()
            }
        }


        Rectangle {
            id : frame
            objectName : uicontext.uniqueName()
            property var resources : []
            property int currentMap : 0

            function setResource(){
                if ( frame.resources.length > 0 && frame.currentMap < frame.resources.length){
                    imageLoader.sourceComponent = null
                    imageLoader.sourceComponent = layerDrawer
                    imageLoader.item.setResource(frame, frame.resources[currentMap])
                }
            }

            function makeResourceList() {
                var list = mastercatalog.selectedData
                frame.resources = []
                currentMap = 0
                progress.maximumValue = list.length
                for(var i=0; i < list.length; ++i){
                    var resource = list[i]
                    if ( resource.typeName === "rastercoverage" ||
                            resource.typeName === "featurecoverage" ||
                            resource.typeName === "pointcoverage" ||
                            resource.typeName === "linecoverage" ||
                            resource.typeName === "polygoncoverage")
                        frame.resources.push(resource)
                }
                setResource()


            }

            function finalizeDraw() {
                frame.resources[currentMap].unload()
                ++currentMap
                progress.value = currentMap
                setResource()
            }

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
            }
            ProgressBar{
                id : progress
                height : frame.height
                width : 15
                anchors.left: frame.right
                anchors.top : frame.top
                orientation: Qt.Vertical
                minimumValue: 0
                maximumValue: 0
            }

        }

    }



}
