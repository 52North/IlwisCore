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
    width : 500
    height : 0
    x : 4
    clip:true

    Column {
        id : dataarea
        function isCsy(objid){
            var tp = mastercatalog.id2type(objid)
            return (tp === "coordinatesystem" || tp === "conventionalcoordinatesystem" || tp === "boundsonlycoordinatesystem")
        }

        width : parent.width - 5
        height : 400
        spacing : 4
        EditorHeader{}
        IlwisObjectCommon{
            id : objectcommon
        }
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
               var createinfo = { name : objectcommon.itemname,
                    type : "coordinatesystem",
                    subtype : "projected",
                    projection : projectionParams.name,
                    projectionparameters : projectionParams.projectionInfo(),
                    ellipsoid : ellipsoidselector.name,
                    envelope : csyBounds.getEnvelope(),
                    datumshifts : datumshifts.text
                }
                objectcreator.createObject(createinfo)
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
                dropItem.state = "invisible"
            }
        }
    }
}
