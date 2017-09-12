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

    function currentCatalogCorrectUrl(){ // must be a file location
        var panel = datapanel.activeItem
        if ( !panel)
            return ""
        if ( panel.panelType === "catalog"){
            var url = panel.currentCatalog.isFileBased ? panel.currentCatalog.container : panel.currentCatalog.url
            if ( url.indexOf("file://") !== 0) {
                createButton.visible = false
                return ""
            }else
                createButton.visible = true
            return url
        }
        createButton.visible = false
        return ""
    }

    Item {
        x : 4
        width : parent.width - 5
        height : 750
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

            Controls.TextEditLabelPair{
                id : keywords
                width : parent.width
                labelText: qsTr("Keywords")
                labelWidth: 100


            }

            Column {
                ListModel {
                    id : inputparameterList
                }
                ListModel {
                    id : outputparameterList
                }
                width : parent.width
                height : 350
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
                    height : 120
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
                width : 250
                spacing : 5
                Text {
                    id : wronglocation
                    width :150
                    height : parent.height
                    text : qsTr("Location can't be used for writing")
                    visible : currentCatalogCorrectUrl() === "" ? true : false
                    color : "red"

                }
                Button {
                    id : createButton
                    text : qsTr("Create & Open")
                    width : parent.width
                    height : 18
                    enabled: nameedit.content.length > 0
                    onClicked: {

                        var inputparms = []
                        for(var i=0; i < inputparameterList.count; ++i){
                            var parm = inputparameterList.get(i)
                            inputparms.push({"name" : parm.name, "valuetype" : parm.valuetype,"description" : parm.description })
                        }
                        var outputparms = []
                        for(var i=0; i < outputparameterList.count; ++i){
                            parm = outputparameterList.get(i)
                            outputparms.push({"name" : parm.name, "valuetype" : parm.valuetype,"description" : parm.description })
                        }
                        var kw = keywords.content
                        if ( kw.indexOf("python") == -1){
                            if ( kw != "")
                                kw += ","
                            kw += "python"
                        }


                        var name = nameedit.content
                        if ( name.indexOf(".py") == -1)
                            name = name + ".py"
                        var createInfo = {
                            type : "script",
                            name : name,

                            keywords : kw,
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

