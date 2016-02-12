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
        GroupBox {
            title: "Projected Coordinate system create methods"
            RowLayout {
                ExclusiveGroup {
                    id: group
                    onCurrentChanged: {
                        if( epsg.checked){
                            proj4def.height = 0
                            proj4def.visible = false
                            fromBase.state = "invisible"
                            epsgnumber.height = Global.rowHeight
                            epsgnumber.visible = true
                        }
                        if ( proj4.checked){
                            proj4def.height = Global.rowHeight
                            proj4def.visible = true
                            fromBase.state = "invisible"
                            epsgnumber.height = 0
                            epsgnumber.visible = false
                        }
                        if ( full.checked){
                            proj4def.height = 0
                            proj4def.visible = false
                            fromBase.state = "visible"
                            epsgnumber.height = 0
                            epsgnumber.visible = false
                        }
                    }
                }
                RadioButton {
                    id : epsg
                    text: "By EPSG number"
                    checked: true
                    exclusiveGroup: group
                }
                RadioButton {
                    id : proj4
                    text: "By Proj4 Defintion"
                    exclusiveGroup: group
                }
                RadioButton {
                    id : full
                    text: "Full definition"
                    exclusiveGroup: group
                }
            }
        }
        CreateProjectionFromBase {
            id : fromBase
            width : parent.width - 10
            height : 0
            maxHeight: parent.height - objectcommon.height
        }
        Controls.TextEditLabelPair {
            id : epsgnumber
            labelText: qsTr("EPSG number")
            labelWidth: 140
            width : parent.width
            regexvalidator: /^\d*/
            visible : false
            height : 0
        }
        Controls.TextEditLabelPair {
            id : proj4def
            labelText: qsTr("Proj4 definition")
            labelWidth: 140
            width : parent.width
            visible : false
            height : 0
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
                var createInfo
                if ( fromBase.state == "visible")
                    createInfo = fromBase.getCreateInfo()
                else if ( epsgnumber.visible){
                    createInfo = { name : objectcommon.itemname,
                        type : "coordinatesystem",
                        subtype : "projected",
                        epsg : epsgnumber.content }
                }else if ( proj4def.visible){
                    createInfo = { name : objectcommon.itemname,
                        type : "coordinatesystem",
                        subtype : "projected",
                        proj4 : proj4def.content }
                }

                objectcreator.createObject(createInfo)
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
