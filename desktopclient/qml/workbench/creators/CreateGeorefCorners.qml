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
    width : 250
    height : 0
    x : 4
    clip:true

    function setValue(type, value){
        if ( type === "coordinatesystem"){
            var resource = mastercatalog.id2Resource(value, dataarea)
            csypart.content = resource.url
        }
    }

    Column {
        id : dataarea
        function isCsy(objid){
            var tp = mastercatalog.id2type(objid)
            return (tp === "coordinatesystem" || tp === "conventionalcoordinatesystem" || tp === "boundsonlycoordinatesystem")
        }

        width : parent.width - 5
        height : 300
        spacing : 4
        EditorHeader{}
        IlwisObjectCommon{
            id : objectcommon
        }
        Connections {
            target : csypart
            onCreateClicked : {
                editorList.push({item:creatorContainer, properties:{creator:objectcreator.creatorInfo("projectedcoordinatesystem")}})
            }
        }



        Controls.FilteredTextEditLabelPair{
            id : csypart
            labelWidth: 120
            labelText: qsTr("Coordinate System")
            filterImage: "../images/csy20.png"
            filterType: "coordinatesystem"
            width : parent.width
            useCreateButton: true

            onIlwisobjectidChanged: {
                var obj = mastercatalog.id2object(ilwisobjectid, csypart)
                if ( obj){
                    csyBounds.islatlon = !obj.isProjected
                }
            }
        }

        CheckBox{
            id : cbcorners
            text : qsTr("Center of Pixels")
            style : Base.CheckBoxStyle1{}
        }
        Controls.TextEditLabelPair{
            id : pixsz
            labelText: qsTr("Pixel size")
            labelWidth: 120
            width : parent.width
            regexvalidator: /^\d*(\.\d*)?$/
        }
        Text {
            text : qsTr("Coordinate Envelope");
            height : Global.rowHeight
            width : parent.width
            font.bold: true
            visible: pixsz.content != ""

        }

        Controls.CoordinateSystemBounds{
            id : csyBounds
            height : Global.rowHeight * 4
            width : parent.width
            visible: pixsz.content != ""
            enabled : visible
        }

    }
    function apply(overwrite) {
        if (!overwrite){
            if ( mastercatalog.exists("ilwis://internalcatalog/"+ objectcommon.itemname, "georeference")){
                return false;
            }
        }
        var url = applyBut.currentCatalogCorrectUrl() + "/"+ objectcommon.itemname
        var createinfo = { name : url, type : "georef", subtype : "corners", minx : csyBounds.minx, miny : csyBounds.miny,
            maxx : csyBounds.maxx, maxy : csyBounds.maxy, csy : csypart.content,
            centered : cbcorners.checked, pixelsize : pixsz.content,
            description :objectcommon.description}
        var createdId = objectcreator.createObject(createinfo)
        if ( createdId !== "?" && editorList.depth > 1){
            editorList.currentItem.setValue("georeference", createdId)
        }
        return true
    }

    ApplyCreateButtons {
        width : parent.width
        height : 60
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        id : applyBut
        createObject: dropItem.apply

    }
}

