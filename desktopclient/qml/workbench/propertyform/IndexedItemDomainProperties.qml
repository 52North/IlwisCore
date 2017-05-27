import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../Global.js" as Global

Rectangle {
    anchors.fill : parent
      color : uicontext.lightestColor

    function storeData() {
        if ( propertyForm.editable){
        }
    }

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
    AttributeProperties {
        anchors.top : parentDomainItem.bottom
        width : parent.width
        domainUsed: domainName
        valueTypeUsed: getProperty("valuetype")
        domainTypeUsed: "IndexedItemDomain"
    }
}
