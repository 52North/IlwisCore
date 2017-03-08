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
        id : inner
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
                regexvalidator: /^-?\d*(\.\d*)?$/
                content : "0"
            }

            Controls.TextEditLabelPair{
                id : maxvalue
                labelText: qsTr("Maximum value")
                labelWidth: 100
                width : parent.width
                regexvalidator: /^-?\d*(\.\d*)?$/
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
                enabled : parentdomtxt.content != ""
                style : Base.CheckBoxStyle1{}
            }

        }
        ApplyCreateButtons {
            width : parent.width
            height : 30
            anchors.bottom : parent.bottom
            anchors.bottomMargin: 8
            anchors.rightMargin: 3
            id : apply

            createObject: inner.apply
        }

        function apply(overwrite) {
            if (!overwrite){
                if ( mastercatalog.exists("ilwis://internalcatalog/"+ namevalue.content, "numericdomain")){
                    return false;
                }
            }
           var url = apply.currentCatalogCorrectUrl() + "/"+ namevalue.content
            var createInfo = {parentdomain : parentdomtxt.content, type : "numericdomain", name :  url, minvalue : minvalue.content, maxvalue : maxvalue.content, resolutionvalue : resvalue.content, description : descvalue.content,strict : cbstrict.checked}
            var ilwisid = objectcreator.createObject(createInfo)
            if ( ilwisid !== "?" && editorList.depth > 1)
                editorList.currentItem.setValue("domain", ilwisid)
            return true
        }
    }
}

