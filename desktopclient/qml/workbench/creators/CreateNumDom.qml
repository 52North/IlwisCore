import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Controls.DropableItem{
    id : dropItem
    width : 275
    height : 0
    clip:true


    Rectangle {
        height: parent.height
        border.width : 1
        border.color : Global.edgecolor
        radius: 5
        width : parent.width

        Column {
            width : parent.width - 7
            height : 210
            y : 5
            spacing : 2
            x : 3

            EditorHeader{}

            Controls.TextEditLabelPair{
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }

            Controls.TextEditLabelPair{
                labelText: qsTr("Minimum value")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^\d*(\.\d*)?$/
            }

            Controls.TextEditLabelPair{
                labelText: qsTr("Maximum value")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^\d*(\.\d*)?$/
            }

            Controls.TextEditLabelPair{
                labelText: qsTr("Resolution")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^\d*(\.\d*)?$/
            }
            Controls.TextAreaLabelPair{
                labelText: qsTr("Description")
                width : parent.width
                height : 40
                labelWidth: 100
            }
            Item {
                width : parent.width
                height : 30
                Button {
                    id : applybutton
                    anchors.right: parent.right
                    width : 70
                    text : qsTr("Apply")
                    y : 10

                }
                Button {
                    id : closebutton
                    anchors.right: applybutton.left
                    anchors.rightMargin: 5
                    width : 70
                    text : qsTr("Close")
                    y : 10

                    onClicked: {
                        dropItem.state = "invisible"
                    }
                }
            }
        }
    }
}

