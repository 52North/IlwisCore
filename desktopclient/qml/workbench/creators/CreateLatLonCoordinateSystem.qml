import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import UIContextModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Controls.DropableItem{
    id : dropItem
    width : 320
    height : 0
    x : 4
    clip:true

    Column {
        id : dataarea
        spacing : 6
        function isCsy(objid){
            var tp = mastercatalog.id2type(objid)
            return (tp === "coordinatesystem" || tp === "conventionalcoordinatesystem" )
        }

        width : parent.width - 5
        height : 400
        EditorHeader{}
        IlwisObjectCommon{
            labelWidth: 140
            id : objectcommon
        }

        Controls.EllipsoidSelector{
            id : ellipsoidselector
            width : parent.width - 10
            labelWidth: 140
            property string name : ""
            onSelectedObjectidChanged : {
                var ilwobj = mastercatalog.id2object(selectedObjectid,ellipsoidselector)
                if ( ilwobj){
                    ellipsoidselector.name = ilwobj.name
                }
            }
        }
        Controls.CoordinateSystemBounds{
            id : csyBounds
            height : Global.rowHeight * 4.5
            width : parent.width
            islatlon: true
            clip : true


            Component.onCompleted: {
                setEnvelope("-180 -80 180 80")
            }
        }
        Item {
            height : Global.rowHeight
            width : parent.width
            CheckBox{
                id : cbshifts
                text : qsTr("Datum shifts (toWgs84)")
                style : Base.CheckBoxStyle1{}
                width : 140

            }
            TextField{
                id : datumshifts
                width : parent.width - cbshifts.width - 10
                height : Global.rowHeight
                anchors.left: cbshifts.right
                enabled: cbshifts.checked
                opacity : cbshifts.checked ? 1 : 0.5
                style: TextFieldStyle {
                    background: Rectangle {
                        radius: 2
                        width : parent.width
                        height: parent.height
                        border.color: Global.edgecolor
                        color : "transparent"
                    }
                }
            }
        }
    }
    function apply(overwrite) {
        if (!overwrite){
            if ( mastercatalog.exists("ilwis://internalcatalog/"+ objectcommon.itemname, "coordinatesystem")){
                return false;
            }
        }
        var url = applyButton.currentCatalogCorrectUrl() + "/"+ objectcommon.name
        var createinfo = { name : url,
            type : "coordinatesystem",
            subtype : "conventional",
            projection : "WGS 84",
            ellipsoid : ellipsoidselector.name,
            envelope : csyBounds.getEnvelope(),
            datumshifts : datumshifts.text
        }

        objectcreator.createObject(createinfo)

        return true
    }

    ApplyCreateButtons {
        width : parent.width
        height : 60
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        id : applyButton
        createObject: dropItem.apply

    }
}

