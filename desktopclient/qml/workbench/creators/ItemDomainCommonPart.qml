import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Column {
    width : parent.width - 7
    height : parent.height
    y : 5
    spacing : 4
    x : 3
    property string valuetype
    property string domaintype
    property string parentItemList
    property string newItemEditor
    property alias parentdomain : parentdomtxt.content
    property alias domitems : domainitems
    property alias additionalFields : extraitems
    property alias name : objectcommon.itemname
    property alias description : objectcommon.description
    property alias strict : cbstrict.checked

    EditorHeader{}

    Item {
        id : parentdom
        width : parent.width
        height : 20
        Text{
            id : parentLabel
            width : 120
            height : 20
            text : qsTr("Parent domain")
        }
        function isCorrectDomain(objid){
            var tp = mastercatalog.id2type(objid)
            container.parentDomain = mastercatalog.id2object(objid, parentdom)
            if ( container.parentDomain && tp === domaintype){
                return container.parentDomain.valuetype === valuetype
            }else{
                container.parentDomain = null
                parentdomtxt.content = ""

            }
            return false
        }

        Controls.TextFieldDropArea{
            id : parentdomtxt
            anchors.left : parentLabel.right
            anchors.right: parent.right
            anchors.rightMargin: 4
            height: 20

            canUse: parentdom.isCorrectDomain
            readOnly: false
            asName: false

            onContentChanged: {
                if ( content != ""){
                    itemloader.source = ""
                    itemloader.source = parentItemList
                }
                else
                    dropItem.state = "invisible"
            }
        }
    }
    IlwisObjectCommon{
        id : objectcommon
    }

    Loader {
        id : extraitems
        width : parent.width
    }

    CheckBox{
        id : cbstrict
        text: qsTr("Strict")
        checked: true
        enabled : parentdomtxt.content != ""
        style : Base.CheckBoxStyle1{}
    }
    Text {
        text : qsTr("Current items")
        font.bold: true
    }
    Rectangle {
        id : itemList
        width : parent.width
        height : 120
        border.width: 1
        border.color : Global.edgecolor
        radius : 3

        Loader{
            id : domainitems
            width : parent.width
            height : parent.height
        }

    }
    Loader {
        id : itemloader
        width : parent.width
        source : newItemEditor
        height : 170
        onSourceChanged: {
            if ( item && container.parentDomain !== undefined){
                item.itemdomain = container.parentDomain
            }
        }
    }
}

