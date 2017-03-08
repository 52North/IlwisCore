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

    x : 4
    clip:true

    function getCreateInfo() {
         var url = apply.currentCatalogCorrectUrl() + "/"+ objectcommon.name
        var createinfo = { name : url,
            type : "coordinatesystem",
            subtype : "conventional",
            projection : projectionParams.name,
            projectionparameters : projectionParams.projectionInfo(),
            ellipsoid : ellipsoidselector.name,
            envelope : csyBounds.getEnvelope(),
            datumshifts : datumshifts.text
        }
        return createinfo;
    }

    Column {
        width : parent.width
        height : parent.height

        Controls.CreateProjection{
            id : projectionParams
        }
        CheckBox{
            id : cbenvelope
            text : qsTr("Define Envelope")
            style : Base.CheckBoxStyle1{}
        }
        Controls.CoordinateSystemBounds{
            id : csyBounds
            height : Global.rowHeight * 2.5
            width : cbenvelope.checked ? parent.width : 0
            opacity: cbenvelope.checked ? 1 : 0
            enabled : opacity != 0
            clip : true
        }
        Controls.EllipsoidSelector{
            id : ellipsoidselector
            width : parent.width - 10
            property string name : ""
            onSelectedObjectidChanged : {
                var ilwobj = mastercatalog.id2object(selectedObjectid,ellipsoidselector)
                if ( ilwobj){
                    ellipsoidselector.name = ilwobj.name
                }
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
}

