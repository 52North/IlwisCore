import QtQuick 2.0

ComboxLabelPair {
    anchors.fill: parent
    property var domain

    onDomainChanged: {
        if ( domain.typeName !== "itemdomain")
            return
        var items = domain.domainitems
        var itemNames = []
        for(var i=0; i < items.length; ++i){
            itemNames.push(items[i].name)
        }
    }
}
