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
    color : Global.formBackGround

    function storeData() {
        if ( propertyForm.editable){
            attributes.storeData()
        }
    }
    function layersCount(){
        var dims = dimensions
        var parts = dims.split(" ")
        if ( parts.length === 3)
            return parts[2]
        return 1
    }

    AttributeProperties{
        id : attributes
        width : parent.width
        domainUsed: domainName
        valueTypeUsed: valuetype
        domainTypeUsed: domainType
    }
    Controls.TextEditLabelPair {
        id : layerCount
        anchors.top : attributes.bottom
        anchors.topMargin: 5
        content:  layersCount()
        labelText: qsTr("Number of layers")
        labelWidth: 100
        readOnly: true

    }

    Item {
        id : layers
        anchors.top: layerCount.bottom
        width : parent.width
        height : 250
        TableView {
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
    }
    Button {
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
