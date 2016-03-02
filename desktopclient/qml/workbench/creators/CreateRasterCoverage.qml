import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0

import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base


Controls.DropableItem{
    id : dropItem
    width : 275
    height : 0
    clip:true
    property var list

    function setValue(type, value){
        var resource
        if ( type === "georeference"){
            resource = mastercatalog.id2Resource(value)
            grfvalue.content = resource.url
        }
        if ( type === "domain"){
            resource = mastercatalog.id2Resource(value)
            domvalue.content = resource.url
        }
    }

    Rectangle {
        height: parent.height
        border.width : 1
        border.color : Global.edgecolor
        radius: 5
        width : parent.width

        Column {
            id : maincolumn
            width : parent.width - 7
            height : 240
            y : 5
            spacing : 2
            x : 4

            EditorHeader{}

            Controls.TextEditLabelPair{
                id : namevalue
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }

            Connections {
                target : grfvalue
                onCreateClicked : {
                    editorList.push({item:creatorContainer, properties:{creator:objectcreator.creatorInfo("cornersgeoreferences")}})
                }
            }

            Connections {
                target : domvalue
                onCreateClicked : {
                    editorList.push({item:creatorContainer, properties:{creator:objectcreator.creatorInfo("domain")}})
                }
            }

            Controls.FilteredTextEditLabelPair{
                id : grfvalue
                labelWidth: 100
                labelText: qsTr("Georeference")
                filterImage: "../images/georeference20.png"
                filterType: "georeference"
                width : parent.width
                useCreateButton: true
            }

            Controls.FilteredTextEditLabelPair{
                id : domvalue
                labelWidth: 100
                labelText: qsTr("Domain")
                filterImage: "../images/domain.png"
                filterType: "domain"
                width : parent.width
                useCreateButton: true
                content : "value"
            }
            Controls.FilteredTextEditLabelPair{
                id : stackdomvalue
                labelWidth: 100
                labelText: qsTr("Stack Domain")
                filterImage: "../images/domain.png"
                filterType: "domain"
                width : parent.width
                useCreateButton: true
                content : "count"
            }

            Controls.TextEditLabelPair{
                id :bandsvalue
                labelText: qsTr("Stack Defintion")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^\d*/
                content : "1"
            }
            Text{
                height: Global.rowHeight - 4
                text : qsTr("Bands")
                font.bold: true
            }


            Rectangle {
                id : bands
                width : parent.width
                height : 90
                DropArea {
                    ListModel {
                        id : rasters
                    }

                    anchors.fill: parent
                    ListView {
                        function bands() {
                            var result
                            for(var i=0; i < rasters.count; ++i){
                                if ( result != "")
                                    result += ","
                                result += rasters.get(i)
                            }
                            return result
                        }

                        id : rasterlist
                        anchors.fill : parent
                        anchors.margins: 2
                        model : rasters
                        delegate :  Text { height : 16; text : path}
                    }
                    onDropped : {
                        var id = drag.source.ilwisobjectid
                        var obj = mastercatalog.id2object(id, rasterlist)
                        console.debug(drag.source.ids,obj.typeName,drag.source.ilwisobjectid)
                        if ( obj.typeName === "rastercoverage"){
                            if ( grfvalue.content === ""){
                                var grf = mastercatalog.id2object(id, rasterlist)
                                if ( grf)
                                    grfvalue.content = grf.url;
                            }
                            if (resampleCB.checked || mastercatalog.isCompatible(grfvalue.content, obj.url, "georeference"))
                                rasters.append({path : obj.url})
                        }
                    }
                }
                border.width: 1
                border.color: Global.edgecolor

            }
            Item {
                width : 100
                height : Global.rowHeight
                CheckBox{
                    id : resampleCB
                    anchors.fill: parent
                    text : qsTr("Auto resample")
                    checked : false
                }
            }

        }
        Item {
            width : parent.width
            height : 30
            anchors.bottom : parent.bottom
            anchors.bottomMargin: 8
            anchors.rightMargin: 3
            Button {
                id : applybutton
                anchors.right: parent.right
                width : 70
                text : qsTr("Apply")
                y : 10
                onClicked: {
                    var createInfo = {georeference : grfvalue.content,
                        domain : domvalue.content,
                        stackdefinition : bandsvalue.content,
                        type : "rastercoverage",
                        name :  namevalue.content,
                        stackdomain : stackdomvalue.content,
                        bands : rasterlist.bands(),
                        autoresample : resampleCB.checked}

                    var ilwisid = objectcreator.createObject(createInfo)
                }

            }
            Button {
                id : closebutton
                anchors.right: applybutton.left
                anchors.rightMargin: 5
                width : 70
                text : qsTr("Close")
                y : 10

                onClicked: {
                    if ( objectcreator.activeCreatorsCount === 1)
                        dropItem.state = "invisible"
                     }
            }
        }
    }
}

