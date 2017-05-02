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
            resource = mastercatalog.id2Resource(value, inner)
            grfvalue.content = resource.url
        }
        if ( type === "domain"){
            resource = mastercatalog.id2Resource(value, inner)
            domvalue.content = resource.url
        }
    }

    Rectangle {
        id : inner
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
                content : "1.."
            }
            Text{
                height: Global.rowHeight - 4
                text : qsTr("Bands")
                font.bold: true
            }
            Row {
                width : parent.width
                height : 22
                spacing : 6
                ExclusiveGroup {
                    id: group
                }
                RadioButton {
                    id : bywildcard
                    text: qsTr("Per Wildcard")
                    exclusiveGroup: group
                }
                RadioButton {
                    id : bydraganddrop
                    text: qsTr("Drag & Drop")
                    exclusiveGroup: group
                    checked: true
                }

            }
            Item {
                width : parent.width
                height : 90
                Controls.TextEditLabelPair{
                    id : bywildcardtext
                    width : parent.width
                    height : Global.rowHeight
                    labelText: qsTr("Wildcards")
                    labelWidth: 120
                    visible: bywildcard.checked
                    enabled: visible
                }

                Rectangle {
                    id : bands
                    width : parent.width
                    height : 90
                    visible : bydraganddrop.checked
                    enabled : visible
                    DropArea {
                        ListModel {
                            id : rasters
                        }

                        anchors.fill: parent
                        ListView {
                            function bands() {
                                var result = ""
                                for(var i=0; i < rasters.count; ++i){
                                    if ( result != "")
                                        result += ","
                                    var r = rasters.get(i)
                                    result += r.path
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
                            var idstring = drag.source.ids
                            if ( (idstring && idstring === "") || idstring.indexOf("|") === -1)
                                idstring = drag.source.ilwisobjectid
                            var ids = idstring.split("|")

                            for(var i=0; i < ids.length; ++i ){
                                var id = ids[i]
                                var obj = mastercatalog.id2object(id, rasterlist)
                                var grfurl = obj.getProperty("georeferenceurl");
                                if ( obj && obj.typeName === "rastercoverage"){
                                    if ( grfvalue.content === ""){
                                        grfvalue.content = grfurl
                                    }
                                    if (resampleCB.checked || mastercatalog.isCompatible(grfvalue.content, grfurl, "georeference"))
                                        rasters.append({path : obj.url})
                                }
                            }
                        }
                    }
                    border.width: 1
                    border.color: Global.edgecolor

                }
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

        function apply(overwrite) {
            if (!overwrite){
                if ( mastercatalog.exists("ilwis://internalcatalog/"+ namevalue.content, "rastercoverage")){
                    return false;
                }
            }
             var url = applyButton.currentCatalogCorrectUrl() + "/"+ namevalue.content
            var createInfo = {georeference : grfvalue.content,
                domain : domvalue.content,
                stackdefinition : bandsvalue.content,
                type : "rastercoverage",
                name :  url,
                stackdomain : stackdomvalue.content,
                bands : bywildcard.checked ? bywildcardtext.content : rasterlist.bands(),
                autoresample : resampleCB.checked}

            objectcreator.createObject(createInfo)


            return true;
        }

        ApplyCreateButtons {
            width : parent.width
            height : 30
            anchors.bottom : parent.bottom
            anchors.bottomMargin: 8
            anchors.rightMargin: 3
            id : applyButton
            createObject: inner.apply
        }
    }
}

