import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import LayersView 1.0
import UIContextModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls


Item {
    Rectangle {
        width: parent.width - 5
        height: parent.height
        x : 5
        y : 5
        border.width: 1
        border.color : Global.edgecolor

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

        Column {
            id : buttonRow
            width : 220
            height : parent.height - 30
            spacing : 6
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
                label : qsTr("Open selected coverages\nin selected panel")
                width : buttonRow.buttonWidth
                height : 40
            }
            Controls.WideButton{
                image : "../images/openfloat20.png"
                label : qsTr("Open selected coverages\nin floating panels")
                width : buttonRow.buttonWidth
                height : 40
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
        Column {
            width : 200
            anchors.left: buttonRow.right
            anchors.leftMargin: 5
            spacing : 6
            Controls.WideButton{
                image : "../images/mapview.png"
                label : qsTr("Generate previews\nof selected coverages")
                width : parent.width
                height : 40

                onClicked: {

                   frame.makeResourceList()
                }
            }

            Image {
                id : page
                width : parent.width - 5
                height : 160
                source : "../../images/page.png"



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

                }
                ProgressBar{
                    id : progress
                    height : page.height - 10
                    width : 15
                    anchors.left: frame.right
                    orientation: Qt.Vertical
                    minimumValue: 0
                    maximumValue: 0
                }
            }
        }

    }
}

