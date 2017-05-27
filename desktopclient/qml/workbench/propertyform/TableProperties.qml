import QtQuick 2.0
import "../../Global.js" as Global

Rectangle {
    function storeData() {
        if ( propertyForm.editable){
        }
    }
      color : uicontext.lightestColor

    Text { id : line1; text : qsTr("Record count"); width: 100; font.italic: true }
    Text { text : getProperty("recordcount");  height : 20;width: parent.width - line1.width - 2; anchors.left: line1.right}
    Text { id : line2; text : qsTr("Column count"); width: 100; font.italic: true; anchors.top : line1.bottom }
    Text { text : getProperty("columncount"); width: parent.width - line2.width - 2; anchors.left: line2.right; anchors.top : line1.bottom}

    Text {
        anchors.top : line2.bottom
        anchors.topMargin: 4
        id : titleText
        text : qsTr("Attributes")
        font.bold: true
        font.pointSize: 9
        height : 25
        verticalAlignment: Text.AlignVCenter
    }
    Rectangle {
        anchors.top : titleText.bottom
        anchors.bottom : parent.bottom
        width:parent.width
        clip : true
        ListView {
            id : attributeList
            anchors.fill: parent
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
                        width : parent.width * 0.74
                        AttributeProperties{
                            id : attProperties
                            width : parent.width
                            domainUsed: attributeDomain
                            valueTypeUsed: attributeValueType
                            columnName : attributename
                        }
                    }
                }
            }
        }
    }
}
