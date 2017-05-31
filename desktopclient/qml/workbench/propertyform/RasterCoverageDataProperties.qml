import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    id : container
    anchors.fill : parent
    color : uicontext.lightestColor

    function storeData() {
        if ( propertyForm.editable){
            dataattributes.storeData()
        }
    }
    function layersCount(){
        var dims = dimensions
        var parts = dims.split(" ")
        if ( parts.length === 3)
            return parts[2]
        return 1
    }

    Column {
        anchors.fill : parent
        spacing: 5
        Rectangle {
            id : rasterAttributes
            width:parent.width
            height : 140
            clip : true
            border.width: 1
            color : uicontext.lightestColor
            border.color : Global.edgecolor
            ListView {
                id : attributeList
                anchors.fill: parent
                anchors.margins: 2
                model : attributes
                delegate {
                    Rectangle {
                        id : myd
                        height : attProperties.height
                        width : parent.width
                        color : index  % 2 == 0 ? "#F7F9FC" : "#DCDCDC"
                        Text{
                            id : attName
                            text : attributename
                            font.bold: true
                            width : parent.width * 0.25
                        }
                        Item {
                            anchors.left: attName.right
                            width : parent.width * 0.749
                            AttributeProperties{
                                id : attProperties
                                width : parent.width
                                domainUsed: attributeDomain
                                valueTypeUsed: attributeValueType
                                domainTypeUsed: attributeDomainType
                                columnName : attributename
                            }
                        }
                    }
                }
            }
        }
        Row {
            height : 20

            width : parent.width
            enabled : hasAttributes
            visible : enabled
            Controls.TextEditLabelPair {
                id : attributeUrl
                height : 20
                width : parent.width - 30
                content:  getProperty("attributetableurl")
                labelText: qsTr("Attribute table")
                labelWidth: 100
                readOnly: true

            }
            Button {
                width : 20
                height : 20
                Image {
                    source : "../../images/table.png"
                    anchors.centerIn: parent
                    width : 16
                    height : 16
                }
                onClicked: {
                    bigthing.newCatalog("resource='" + attributeUrl.content + "'","table",attributeUrl.content, "left")
                    //bigthing.newCatalog(attributeUrl.content ,"table",attributeUrl.content, "left")
                }
            }
        }
        Controls.TextEditLabelPair {
            id : layerCount
            height : 20
            content:  layersCount()
            labelText: qsTr("Number of layers")
            labelWidth: 100
            readOnly: true

        }

        Item {
            width : parent.width
            height : 160
            TableView {
                id : layers
                anchors.fill: parent
                model : layerInfo
                TableViewColumn{
                    id : titleColumn
                    title : qsTr("Layer name");
                    role : "name"
                    width : 200
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
                    id : layerrs
                    title : qsTr("Range");
                    role : "range"
                    width : parent.width - titleColumn.width
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
            Button {
                id : applBut
                anchors.top : layers.bottom
                anchors.topMargin: 3
                anchors.right: parent.right
                anchors.rightMargin: 6
                width : 90
                height : 20
                text : qsTr("Actual range(s)")
                onClicked :{
                    recalcLayers()
                }
            }
        }

    }

}
