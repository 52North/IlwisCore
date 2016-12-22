import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import LayersView 1.0
import SMCE 1.0
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls

Item {
    //width: parent.width
    //height: parent.height

    property SMCE smceModel

       Row {

            Column {
                id: goalColumn
                width: parent.parent.width / 2

                Rectangle {
                    id : goaldetaillabel
                    //height : 18
                    color : Global.palegreen
                    Text{
                        text : qsTr("Goal definition")
                        font.bold : true
                        x : 5
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                GroupBox {
                    id: scale
                    Layout.fillWidth: true
                    ColumnLayout {
                        ExclusiveGroup { id: scaleGroup }
                        RadioButton {
                            id: problemButton
                            text: qsTr("Problem (1 -> 0)")
                            checked: true
                            exclusiveGroup: scaleGroup
                            Layout.minimumWidth: 100
                        }
                        RadioButton {
                            id: opportunityButton
                            text: qsTr("Opportunity (0 -> 1)")
                            exclusiveGroup: scaleGroup
                            Layout.minimumWidth: 100
                        }
                    }
                }

                Controls.TextEditLabelPair{
                    id : unitsfield
                    labelWidth: 80
                    labelText: qsTr("Units")
                    transparentBackgrond: false

                }

                // Analysis area
                Controls.TextEditLabelPair{
                    id : analysisRaster
                    labelWidth: 80
                    labelText: qsTr("Analysis area (raster)")
                    transparentBackgrond: false

                }

                Button {
                    id : applyGoalBut
                    height : 22
                    text : qsTr("Apply")

                    onClicked: {

                    }
                }
            }

            Column {
                id: nodeColumn
                width: parent.parent.width / 2

                Rectangle {
                    id : nodedetaillabel
                    //height : 18
                    color : Global.palegreen
                    Text{
                        text : qsTr("Selected node definition")
                        font.bold : true
                        x : 5
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Controls.ComboxLabelPair{
                    id : nodeType
                    width : parent.width
                    height : 20
                    labelWidth: 100
                    labelText: qsTr("Node type")
                    itemModel: {}
                }


                Controls.TextAreaLabelPair{
                    id : nodeText
                    labelWidth: 80
                    height: 60
                    width: parent.width
                    labelText: qsTr("Text")
                }

                Controls.TextEditLabelPair{
                    id : inpIndicatorRaster
                    labelWidth: 80
                    labelText: qsTr("Input indicator (raster)")
                    transparentBackgrond: false
                }


                Controls.LabeledCheckBox {
                    id: ro
                    labelText : qsTr("Read only")
                }

                Button {
                    id : applyNodeBut
                    height : 22
                    text : qsTr("Apply")
                    onClicked: {

                    }
                }
            }
        }

       function updateNodeInfo(id) {
            nodeText.content = smceModel.getNode(id).name

       }

}
