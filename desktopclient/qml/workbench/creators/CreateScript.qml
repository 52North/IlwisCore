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
        anchors.fill: parent
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

                        var name = nameedit.content
                        var url = textid.text + '/' + name + '.ilwis'
                        var createInfo = {
                            type : "script",
                            name : name,
                            keywords : keywords,
                            description : descedit.content,
                            url : mastercatalog.currentUrl
                        }
                        var ilwisid = objectcreator.createObject(createInfo)
                        if (resource){
                            var filter = "itemid=" + resource.id
                            operations.refresh()
                            bigthing.newCatalog(filter, "script",resource.url,"other")
                        }
                    }
                }
            }
        }
    }
}

