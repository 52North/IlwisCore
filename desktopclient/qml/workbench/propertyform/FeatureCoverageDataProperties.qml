import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0


Item {
    id : container
    anchors.fill : parent


    Text {
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
                        }
                    }
                }
            }
        }
    }
}
