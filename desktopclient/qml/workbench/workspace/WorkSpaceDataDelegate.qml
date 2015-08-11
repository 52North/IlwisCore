import QtQuick 2.0

Item {
    id: iconDelegate
    x: 2
    y : 3
    width :  GridView.view.cellWidth
    height :  GridView.view.cellHeight

    function iconSource(name) {
        if ( name === "")
            name = "redbuttonr.png"
        var iconP = "../../images/" + name
        return iconP

    }
    Row{
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        spacing: 3
        Image{
            id : imageBase
            width :13; height : 13
            source : iconSource(iconPath)
            fillMode: Image.PreserveAspectFit
            z : 0
        }
        Text {
            id: txt
            text: displayName
            elide: Text.ElideMiddle
            width : 140
            y : 4
            font.pointSize: 8
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}

