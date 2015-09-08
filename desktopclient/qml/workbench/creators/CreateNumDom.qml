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
            height : 240
            y : 5
            spacing : 2
            x : 3

            EditorHeader{}

            Item {
                id : parentdom
                width : parent.width
                height : 20
                Text{
                    id : parentLabel
                    width : 100
                    height : 20
                    text : qsTr("Parent domain")
                }
                function isNumericDomain(objid){
                    var tp = mastercatalog.id2type(objid)
                    return tp === "numericdomain";
                }

                Controls.TextFieldDropArea{
                    id : parentdomtxt
                    anchors.left : parentLabel.right
                    anchors.right: parent.right
                    anchors.rightMargin: 4
                    height: 20

                    canUse: parentdom.isNumericDomain
                    readOnly: false
                    asName: false
                }
            }

            Controls.TextEditLabelPair{
                id : namevalue
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }

            Controls.TextEditLabelPair{
                id : minvalue
                labelText: qsTr("Minimum value")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^\d*(\.\d*)?$/
                content : "0"
            }

            Controls.TextEditLabelPair{
                id : maxvalue
                labelText: qsTr("Maximum value")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^\d*(\.\d*)?$/
                content : "100"
            }

            Controls.TextEditLabelPair{
                id : resvalue
                labelText: qsTr("Resolution")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^\d*(\.\d*)?$/
                content : "1"
            }
            Controls.TextAreaLabelPair{
                id : descvalue
                labelText: qsTr("Description")
                width : parent.width
                height : 40
                labelWidth: 100
            }
            CheckBox{
                id : cbstrict
                text: qsTr("Strict")
                checked: true
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
                    onClicked: {
                        dropItem.state = "invisible"
                        var createInfo = {parentdomain : parentdomtxt.content, type : "numericdomain", name :  namevalue.content, minvalue : minvalue.content, maxvalue : maxvalue.content, resolutionvalue : resvalue.content, description : descvalue.content,strict : cbstrict.checked}
                        var ilwisid = objectcreator.createObject(createInfo)
                        console.debug(ilwisid)

                    }

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

