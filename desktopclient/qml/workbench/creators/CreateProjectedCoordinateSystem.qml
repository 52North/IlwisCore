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
                    description :objectcommon.description,
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
