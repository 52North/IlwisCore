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

        Text {
            text : qsTr("Coordinate Envelope");
            height : Global.rowHeight
            width : parent.width
            font.bold: true
        }

        Controls.CoordinateSystemBounds{
            id : csyBounds
            height : Global.rowHeight * 4
            width : parent.width
        }

    }
    function apply(overwrite) {
        if (!overwrite){
            if ( mastercatalog.exists("ilwis://internalcatalog/"+ objectcommon.itemname, "boundsonlycoordinatesystem")){
                return false;
            }
        }
        var url = applyBut.currentCatalogCorrectUrl() + "/"+ objectcommon.itemname
        var createinfo = { name : url, type : "coordinatesystem", subtype : "boundsonly", minx : csyBounds.minx, miny : csyBounds.miny,
            maxx : csyBounds.maxx, maxy : csyBounds.maxy, csy : csypart.content,
            description :objectcommon.description}
        var createdId = objectcreator.createObject(createinfo)
        if ( createdId !== "?" && editorList.depth > 1){
            editorList.currentItem.setValue("coordinatesystem", createdId)
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


