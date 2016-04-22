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
            var resource = mastercatalog.id2Resource(value)
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

        Controls.CoordinateSystemBounds{
            id : csyBounds
            height : Global.rowHeight * 4
            width : parent.width
            visible: pixsz.content != ""
            enabled : visible
        }

    }
    Item {
        width : parent.width
        height : 60
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        Button {
            id : applybutton
            anchors.right: parent.right
            anchors.rightMargin: 6
            anchors.bottom: parent.bottom
            width : 70
            text : qsTr("Apply")
            onClicked: {
                var createinfo = { name : objectcommon.itemname, type : "georef", subtype : "corners", minx : csyBounds.minx, miny : csyBounds.miny,
                    maxx : csyBounds.maxx, maxy : csyBounds.maxy, csy : csypart.content,
                    centered : cbcorners.checked, pixelsize : pixsz.content,
                    description :objectcommon.description}
                var createdId = objectcreator.createObject(createinfo)
                editorList.pop()
                if ( createdId !== "?" && editorList.depth > 1)
                    editorList.currentItem.setValue("georeference", createdId)
            }

        }
        Button {
            id : closebutton
            anchors.right: applybutton.left
            anchors.rightMargin: 6
            anchors.bottom: parent.bottom
            width : 70
            text : qsTr("Close")
            onClicked: {
                if ( editorList.depth <= 1)
                    dropItem.state = "invisible"
                //editorList.pop()
            }
        }
    }
}

