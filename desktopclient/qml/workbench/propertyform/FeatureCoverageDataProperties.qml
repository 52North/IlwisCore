import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../Global.js" as Global


Rectangle {
    id : container
    anchors.fill : parent
    color : uicontext.lightestColor

    function storeData() {
        if ( propertyForm.editable){
        }
    }

    Text { id : line1; text : qsTr("Polygon features"); width: 100; font.italic: true }
    Text { text : getProperty("polygoncount");  height : 20;width: parent.width - line1.width - 2; anchors.left: line1.right}
    Text { id : line2; text : qsTr("Line features"); width: 100; font.italic: true; anchors.top : line1.bottom }
    Text { text : getProperty("linecount"); width: parent.width - line2.width - 2; anchors.left: line2.right; anchors.top : line1.bottom}
    Text { id : line3; text : qsTr("Point features"); width: 100; font.italic: true; anchors.top : line2.bottom }
    Text { text : getProperty("pointcount"); width: parent.width - line3.width - 2; anchors.left: line3.right; anchors.top : line2.bottom}
    Text { id : line4; text : qsTr("Total features"); width: 100; font.italic: true; anchors.top : line3.bottom }
    Text { text : getProperty("featurecount"); width: parent.width - line4.width - 2; anchors.left: line4.right; anchors.top : line3.bottom}


    Text {
        id : titleText
        text : qsTr("Attributes")
        font.bold: true
        font.pointSize: 9
        height : 25
        anchors.top : line4.bottom
        anchors.topMargin: 4
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        anchors.top : titleText.bottom
        anchors.bottom : parent.bottom
        width:parent.width
        clip : true
        border.width: 1
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
}
