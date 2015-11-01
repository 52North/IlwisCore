import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
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
                width : 200
                spacing : 5

                Button {
                    text : qsTr("Create & Open")
                    width : 110
                    height : 18
                    onClicked: {
                        var keywrds = ""
                        for( var i=0; i < keyitems.model.length; ++i){
                            var item = keyitems.model[i]
                            if ( keywrds != ""){
                                keywrds += "|"
                            }
                            keywrds += item.trim()
                        }

                        var createInfo = {type : "workflow", name : nameedit.content, keywords : keywrds, description : descedit.content}
                        var ilwisid = objectcreator.createObject(createInfo)
                        var resource = mastercatalog.id2Resource(ilwisid)
                        if (resource){
                            var filter = "itemid=" + resource.id
                            bigthing.newCatalog(filter, "workflow",resource.url,"other")
                        }
                    }
                }
                Button{
                    id : createBut
                    text : qsTr("Create")
                    width : 84
                    height : 18

                }
            }

        }
    }
}

