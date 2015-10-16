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
        Item {
            height : 100
            width : parent.width
            Text {
                id : projlabel
                height : 20
                width : parent.width
                text : qsTr("Projections")
                font.bold: true
            }

            Rectangle {
                anchors.top : projlabel.bottom
                width : parent.width
                height : 80
                border.width: 1
                radius: 3
                border.color : Global.edgecolor
                ScrollView{
                    anchors.fill: parent
                    ListView {
                        id : projnames
                        anchors.fill: parent
                        clip:true
                        currentIndex: -1
                        model : mastercatalog.select("type=" + uicontext.typeName2typeId("projection"),"name")
                        highlight : Rectangle { height : 16; width : parent.width; color : Global.selectedColor}
                        delegate {
                            Text {
                                width : parent.width
                                height : 16
                                text : modelData.split("|")[1]
                                x : 3
                                font.pointSize: 9

                                MouseArea{
                                    anchors.fill : parent
                                    onClicked: {
                                        projnames.currentIndex = index
                                        var ilwobj = mastercatalog.id2object(modelData.split("|")[0],projnames)
                                        if ( ilwobj){
                                            prjparameters.model = ilwobj.projectionItems;
                                            parmcontainer.state = "maximized"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Item {
            id : parmcontainer
            height : 0
            width : parent.width
            clip : true
            Text {
                id : parmlabel
                height : 20
                width : parent.width
                text : qsTr("Projection Parameters")
                font.bold: true
            }
            Rectangle {
                id : parmborder
                anchors.top : parmlabel.bottom
                anchors.topMargin: 2
                width : parent.width
                height : parmcontainer.height - 25
                border.width: 1
                border.color: Global.edgecolor
                radius: 3


                Controls.ProjectionParametersEditor{
                    id : prjparameters
                    width : parent.width
                    height : parmborder.height
                    anchors.margins: 2
                }
            }
            states: [
                State { name: "maximized"
                    PropertyChanges {
                        target: parmcontainer
                        height : 155
                    }
                },
                State {
                    name : "minimized"
                    PropertyChanges {
                        target: parmcontainer
                        height : 0
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 500 ; easing.type: Easing.InOutCubic }
                }
            ]
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
        //        Row {
        //            height : Global.rowHeight
        //            width : parent.width
        //            Text {
        //                id : ellipsoidlabel
        //                height : parent.height
        //                width : 120
        //                text : qsTr("Ellipsoid")
        //                font.bold: true
        //            }
        Controls.IlwisComboBox{
            width : parent.width
            height : 20
            z : 1

        }
        //            z : 1
        //        }

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
