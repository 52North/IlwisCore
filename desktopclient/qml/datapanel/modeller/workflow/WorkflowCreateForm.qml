import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Item {
    id : createForm
    state : "invisible"
    height : parent.height

    function currentCatalogCorrectUrl(){ // must be a file location
        var panel = datapanel.activeItem
        if ( !panel)
            return ""
        if ( panel.panelType === "catalog"){
            var url = panel.currentCatalog.url
            if ( url.indexOf("file://") !== 0) {
                return ""
            }
            return url
        }
        return ""
    }

    Column {
        width : parent.width - 3
        height : parent.height
        spacing : 3
        x : 3
        Controls.TextEditLabelPair{
            id : namefield
            width : parent.width
            labelWidth: 80
            labelText: qsTr("Name")
            transparentBackgrond: false
        }
        Controls.TextAreaLabelPair{
            id : descField
            width : parent.width
            labelWidth: 80
            height : parent.height - 160
            labelText: qsTr("Description")
        }
         Controls.TextEditLabelPair{
            id : keywordsField
            width : parent.width
            labelText: qsTr("Keywords")
            labelWidth: 100
        }
         Text {
             id : errorField
             height : 20
             width : 200
             text : qsTr("Location can not be used for writing")
             visible : Global.currentCatalogCorrectUrl(centerItem.activeItem) === ""
             color : "red"
         }

        Row {
            width : 200
            height : 22
            spacing : 4
            Button {
                id : applyBut
                width : 100
                height : 22
                enabled :  Global.currentCatalogCorrectUrl(centerItem.activeItem) !== ""
                text : qsTr("Create")
                onClicked: {
                    var path = Global.currentCatalogCorrectUrl(centerItem.activeItem)
                    if (path !== ""){
                        var keywords = keywordsField.content
                        if ( keywords === ""){
                            keywords = "workflow"
                        } else {
                            keywords += ", workflow"
                        }

                        var name = namefield.content
                        if ( path){
                            var url =path + '/' + name + '.ilwis'
                            var createInfo = {
                                type : "workflow",
                                name : name,
                                keywords : keywords,
                                description : descField.content,
                                url : url
                            }
                            var ilwisid = objectcreator.createObject(createInfo)
                            workflows.addWorkflow(ilwisid)
                        }
                    }
                }
            }
            Button {
                width : 100
                height : 22
                text : qsTr("Close")
                onClicked: {
                    createForm.state = "invisible"
                }
            }
        }
    }

    states: [
        State { name: "invisible"

            PropertyChanges {
                target: createForm
                width : 0
            }
            PropertyChanges {
                target: dataForm
                opacity : 1
            }
        },
        State {
            name : "visible"
            PropertyChanges {
                target: createForm
                width : layerprops.width - workflows.width - 3
            }
            PropertyChanges {
                target: dataForm
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "width"; duration : 630 ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : 630 ; easing.type: Easing.InOutCubic }
            onRunningChanged: {
                if ((state == "invisible") && (!running)) {
                    opacity = 0
                }else{
                    opacity = 1
                }

            }
        }
    ]
}

