import QtQuick 2.0

ListView{

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

         var iconP = "../images/" + name
         return iconP
     }

    property int headerHeight : 26

    id: propertiesList
    width: parent.width
    height : parent.height-headerHeight
    y : headerHeight
    opacity : 1
    clip : true

    focus: true
    highlight: Rectangle {
        width: parent.width
        height: parent.height-headerHeight
        color: "Green"
        opacity : 0.2
    }
    delegate: Item {
        id : collapseItem
        height : 20
        width : parent.width



        Text  {
            id : gpText
            width : parent.width
            height : 20
            text : label
            font.pointSize: 9
        }
        Image {
            height : 16; width :16
            source : iconsource(icon)
            anchors.right: parent.right
        }

        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            cursorShape: Qt.ArrowCursor
            onClicked: {
                propertiesList.currentIndex = index
            }
        }

    }
}
