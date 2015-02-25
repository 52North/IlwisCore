import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

Rectangle {
    id : container
    anchors.fill : parent

    function storeData() {
        if ( propertyForm.editable){
            attributes.storeData()
        }
    }

    AttributeProperties{
        id : attributes
        width : parent.width
        domainUsed: domainName
        valueTypeUsed: valuetype
        domainTypeUsed: domainType
    }
}
