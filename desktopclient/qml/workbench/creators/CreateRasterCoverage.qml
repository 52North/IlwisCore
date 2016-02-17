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
            id : maincolumn
            width : parent.width - 7
            height : 240
            y : 5
            spacing : 2
            x : 4

            EditorHeader{}

            Controls.TextEditLabelPair{
                id : namevalue
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }

            Controls.FilteredTextEditLabelPair{
                id : grfvalue
                labelWidth: 100
                labelText: qsTr("Georeference")
                filterImage: "../images/georeference20.png"
                filterType: "georeference"
                width : parent.width
                useCreateButton: true
            }

            Controls.FilteredTextEditLabelPair{
                id : domvalue
                labelWidth: 100
                labelText: qsTr("Domain")
                filterImage: "../images/domain.png"
                filterType: "domain"
                width : parent.width
                useCreateButton: true
            }

            Controls.TextEditLabelPair{
                id :bandsvalue
                labelText: qsTr("Number of bands")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^\d*/
                content : "1"
            }


            Controls.TextAreaLabelPair{
                id : descvalue
                labelText: qsTr("Description")
                width : parent.width
                height : 40
                labelWidth: 100
            }
            Controls.TextEditLabelPair{
                id :keywordsvalue
                labelText: qsTr("Keywords")
                labelWidth: 100
                width : parent.width
            }
        }
        Item {
            width : parent.width
            height : 30
            anchors.bottom : parent.bottom
            anchors.bottomMargin: 8
            anchors.rightMargin: 3
            Button {
                id : applybutton
                anchors.right: parent.right
                width : 70
                text : qsTr("Apply")
                y : 10
                onClicked: {
                    var createInfo = {georeference : grfvalue.content,
                        domain : domvalue.content,
                        bands : bandsvalue.content,
                        keywords : keywordsvalue.content,
                        type : "rastercoverage",
                        name :  namevalue.content,
                        description : descvalue.content}

                    var ilwisid = objectcreator.createObject(createInfo)
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

