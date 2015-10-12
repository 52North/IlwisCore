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
        Row {
            width : parent.width
            height : Global.rowHeight
            Text{
                id : csyLabel
                width : 120
                height : 20
                text : qsTr("Coordinate System")
                font.bold: true
            }

            Controls.TextFieldDropArea{
                id : csypart
                width : parent.width - csyLabel.width - 23
                height: Global.rowHeight

                canUse: dataarea.isCsy
                readOnly: false
                asName: false
                onObjectidChanged: {
                    csyBounds.opacity = content != ""
                    var model = mastercatalog.id2object(csypart.objectid, csypart)
                    if ( model){
                        csyBounds.islatlon = !model.isProjected
                        var bb = model.getProperty(model.isProjected ? "envelope" : "latlonenvelope")
                        console.debug("bb", bb)
                        csyBounds.setBoundingBox(bb)

                    }
                }

            }
            Button {
                height : 20
                width : 20
                Image {
                    id: name
                    source: "../../images/plus20.png"
                    width : 16
                    height : 16
                    anchors.centerIn: parent
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

        Controls.CoordinateSystemBounds{
            id : csyBounds
            height : Global.rowHeight * 4
            width : parent.width
            opacity: 0
            enabled : opacity != 0
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
                var createinfo = { name : objectcommon.itemname, type : "georef", subtype : "corners", minx : csyBounds.minx, miny : csyBounds.miny,
                    maxx : csyBounds.maxx, maxy : csyBounds.maxy, csy : csypart.content,
                    centered : cbcorners.checked, pixelsize : pixsz.content,
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

