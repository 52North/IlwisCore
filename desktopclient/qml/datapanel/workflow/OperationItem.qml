import QtQuick 2.0

Rectangle {
    id : operationItem
    width: 200
    height: 80
    property string name
    property bool isSelected : false
    Image {
        id : box
        anchors.fill: parent
        source : iconsource("operationitem.png")
    }
    Text{
        anchors.top : box.top
        anchors.topMargin: 9
        width : box.width
        x : 15
        text : name
        font.bold : true

    }
    function deselectAll(){
        at1.isSelected = at2.isSelected = at3.isSelected = at4.isSelected = at5.isSelected = at6.isSelected = at7.isSelected = at8.isSelected = false
    }

    AttachmentRectangle{
        id : att1
        anchors.top: operationItem.top
        anchors.topMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/4 - 5
    }
    AttachmentRectangle{
        id : att2
        anchors.top: operationItem.top
        anchors.topMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: 3 * operationItem.width/4
    }
    AttachmentRectangle{
        id : att3
        anchors.top: operationItem.top
        anchors.topMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/2 - 5
    }

    AttachmentRectangle{
        id : att4
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/4 - 5
    }
    AttachmentRectangle{
        id : att5
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: 3 * operationItem.width/4
    }

    AttachmentRectangle{
        id : att6
        anchors.bottom: operationItem.bottom
        anchors.bottomMargin: -2
        anchors.left : operationItem.left
        anchors.leftMargin: operationItem.width/2 - 5
    }

    AttachmentRectangle{
        id : att7
        anchors.left: operationItem.left
        anchors.leftMargin: 2
        anchors.top : operationItem.top
        anchors.topMargin: operationItem.height/2
    }

    AttachmentRectangle{
        id : att8
        anchors.right: operationItem.right
        anchors.rightMargin: 0
        anchors.top : operationItem.top
        anchors.topMargin: operationItem.height/2
    }
}

