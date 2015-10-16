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
    width : 550
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
        Rectangle {
            width : parent.width
            height : 200
            border.width: 1
            border.color : Global.edgecolor
            ScrollView{
                anchors.fill: parent
                ListView {
                    id : projnames
                    anchors.fill: parent
                    clip:true
                    currentIndex: -1
                    model : mastercatalog.select("type=" + uicontext.typeName2typeId("projection"),"name")
                    highlight : Rectangle { height : 18; width : parent.width; color : Global.selectedColor}
                    delegate {
                        Text {
                            width : parent.width
                            height : 18
                            text : modelData.split("|")[1]
                            x : 3

                            MouseArea{
                                anchors.fill : parent
                                onClicked: {
                                    projnames.currentIndex = index
                                    var ilwobj = mastercatalog.id2object(modelData.split("|")[0],projnames)
                                    if ( ilwobj){
                                        prjparameters.model = ilwobj.projectionItems;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        CheckBox{
            id : cbenvelope
            text : qsTr("Define Envelope")
            style : Base.CheckBoxStyle1{}
        }
        Controls.CoordinateSystemBounds{
            id : csyBounds
            height : Global.rowHeight * 4
            width : cbenvelope.checked ? parent.width : 0
            opacity: cbenvelope.checked ? 1 : 0
            enabled : opacity != 0
            clip : true
        }
        Controls.ProjectionParametersEditor{
            id : prjparameters
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
                dropItem.state = "invisible"
                var points
                var createinfo = { name : objectcommon.itemname, type : "coordinatesystem", subtype : "projected",
                    description :objectcommon.description}
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
