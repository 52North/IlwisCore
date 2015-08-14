import QtQuick 2.0

Rectangle {
    width: 8
    height: 8
    border.width: 1
    opacity: operationItem.isSelected ? 1.0 : 0
    enabled : operationItem.isSelected ? true : false
    border.color : isSelected ? "red": "#A6B8C9"
    color : "transparent"
    property bool isSelected : false
    z : 1
    MouseArea{
        anchors.fill: parent
        onClicked: {
            deleselectAll()
            isSelected = true;
            console.debug("clicked")
        }
    }
}

