import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import IlwisObjectCreatorModel 1.0

import "../.." as Base
import "../../workbench" as Workbench
import "../../Global.js" as Global
import "../../controls" as Controls

Controls.DropableItem{
    width : parent.width
    height : 0
    clip:true

    property var keylist : []
    property bool isNew : true

    Item {
        x : 4
        width : parent.width - 5
        height : parent.height
        Column {
            id : workflowItems
            anchors.fill: parent
            spacing : 4

            Controls.TextEditLabelPair{
                id : nameedit
                labelText: qsTr("Name")
                labelWidth: 100
                width : parent.width
            }
            Controls.TextAreaLabelPair{
                id : descedit
                labelText: qsTr("Description")
                labelWidth: 100
                height : 60
                width : parent.width
            }
            Item {
                width : parent.width
                height : 90
                Text {
                    id : label
                    text : qsTr("Keywords")
                    height : 20
                    width : 100
                    font.bold: true
                }

                Rectangle {
                    id : itemList
                    anchors.left: label.right
                    width : parent.width - 100
                    height : 60
                    border.width: 1
                    border.color : Global.edgecolor
                    radius : 3
                    ScrollView{
                        anchors.fill : parent
                        ListView {
                            id : keyitems
                            anchors.fill : parent
                            model : keylist
                            highlight : Rectangle{width : 80;height : 18;color : Global.selectedColor}
                            delegate: Component {
                                Text{
                                    x : 4
                                    text : modelData
                                    width :80
                                    height : 18
                                    MouseArea{
                                        anchors.fill: parent
                                        onClicked: {
                                            keyitems.currentIndex = index
                                        }
                                    }
                                }

                            }
                        }
                    }
                }
                Row {
                    anchors.top : itemList.bottom
                    anchors.topMargin: 3
                    x : 100
                    width : parent.width - 100
                    height : 20
                    spacing : 5
                    Button {
                        width : 60
                        height : 18
                        text : qsTr("Delete")
                        onClicked: {
                            keylist.splice(keyitems.currentIndex,1)
                            keyitems.model = keylist
                        }
                    }

                    Controls.ComboxLabelPair{
                        id : keys
                        content : "test"
                        labelText: ""
                        labelWidth: 10
                        width : parent.width - 90
                        itemModel: operations.keywords
                    }
                    Button{
                        text : qsTr("+")
                        width : 20
                        height : 20
                        onClicked: {
                            keylist.push(keys.comboText)
                            keyitems.model = keylist
                        }
                    }
                }
            }

            Column {
                ListModel {
                    id : inputparameterList
                }
                ListModel {
                    id : outputparameterList
                }
                width : parent.width
                height : 250
                spacing : 3
                CheckBox{
                    id : operationCheck
                    text : qsTr("As Operation")
                    checked: false
                }
                Controls.TextEditLabelPair{
                    id : operationNameField
                    width : parent.width
                    labelText: qsTr("Operation Name")
                    labelWidth: 100
                    visible: operationCheck.checked
                    enabled: visible
                }
                Controls.TextEditLabelPair{
                    id : longNameField
                    width : parent.width
                    labelText: qsTr("Long name")
                    labelWidth: 100
                    visible: operationCheck.checked
                    enabled: visible
                }

                ParameterList{
                    id : parameterlistField
                    visible: operationCheck.checked
                    enabled: visible
                    width : parent.width
                    height : 80
                }
                Button{
                    anchors.right : parent.right
                    anchors.rightMargin: 5
                    visible: operationCheck.checked
                    enabled: visible
                    width : 100
                    height : 20
                    text : qsTr("New Parameter")
                    onClicked: {
                        details.visible = true;
                    }
                }
                ParameterDetails{
                    id : details
                    visible: false
                    enabled: visible
                    width : parent.width
                    height : 100
                }


            }
            Rectangle{
                x : 2
                width : parent.width -4
                height : 1
                color : Global.edgecolor
            }
            Row{
                anchors.right: parent.right
                height : 22
                width : 110
                spacing : 5

                Button {
                    id : createButton
                    text : qsTr("Create & Open")
                    width : parent.width
                    height : 18
                    enabled: nameedit.content.length > 0
                    onClicked: {
                        var keywords = ""
                        for( var i=0; i < keyitems.model.length; ++i){
                            var item = keyitems.model[i]
                            if ( keywords != ""){
                                keywords += ", "
                            }
                            keywords += item.trim()
                        }
                        if ( keywords === ""){
                            keywords = "script"
                        } else {
                            keywords += ", script"
                        }
                        var inputparms = []
                        for(i=0; i < inputparameterList.count; ++i){
                            var parm = inputparameterList.get(i)
                            inputparms.push({"name" : parm.name, "valuetype" : parm.valuetype,"description" : parm.description })
                        }
                        var outputparms = []
                        for(i=0; i < outputparameterList.count; ++i){
                            parm = outputparameterList.get(i)
                            outputparms.push({"name" : parm.name, "valuetype" : parm.valuetype,"description" : parm.description })
                        }

                        var name = nameedit.content
                        if ( name.indexOf(".py") == -1)
                            name = name + ".py"
                        var createInfo = {
                            type : "script",
                            name : name,
                            keywords : keywords,
                            description : descedit.content,
                            url : mastercatalog.currentUrl + "/" + name,
                            asoperation : operationCheck.checked,
                            inputparameters : inputparms,
                            outputparameters : outputparms,
                            operationname : operationNameField.content,
                            longname : longNameField.content
                        }
                        var ilwisid = objectcreator.createObject(createInfo)
                        var filter = "itemid=" + ilwisid
                        operations.refresh()
                        bigthing.newCatalog(filter, "script", mastercatalog.currentUrl + "/" + name,"other")
                    }
                }
            }
        }
    }
}

