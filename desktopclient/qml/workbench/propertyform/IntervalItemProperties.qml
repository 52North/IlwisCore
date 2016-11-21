import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ResourceModel 1.0
import IlwisObjectModel 1.0
import DomainItemModel 1.0

import "../../Global.js" as Global

Item {
    function storeData() {
        if ( propertyForm.editable){
        }
    }

    anchors.fill : parent
    Item {
        id : parentDomainItem
        width : parent.width
        height : Global.rowHeight
        Text{
            id : label1
            width : 100
            height : Global.rowHeight
            text : qsTr("Parent Domain")
        }
        Text{
            anchors.left:label1.right
            width : parent.width - label1.width
            height :Global.rowHeight
            text : getProperty("parentdomain")
        }
    }
    Item {
        id : domainItems
        anchors.top: parentDomainItem.bottom
        width : parent.width
        height : parent.height - 20
        TableView {
            anchors.fill: parent
            model : domainitems
            TableViewColumn{
                title : qsTr("Item name");
                role : "name"
                width : 100
                delegate: Component{
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                    }
                }
            }
            TableViewColumn{
                title : qsTr("Code");
                role : "code"
                width : 40
                delegate: Component{
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                    }
                }
            }
            TableViewColumn{
                title : qsTr("Minimum");
                role : "minimum"
                width : 70
                delegate: Component{
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                    }
                }
            }
            TableViewColumn{
                title : qsTr("Maximum");
                role : "maximum"
                width : 70
                delegate: Component{
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                    }
                }
            }
            TableViewColumn{
                title : qsTr("Description");
                role : "description"
                width : parent ? parent.width - 140 : 0
                delegate: Component{
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                    }
                }
            }

        }
    }
}

