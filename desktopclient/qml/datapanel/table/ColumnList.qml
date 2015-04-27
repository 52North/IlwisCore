import QtQuick 2.0
import "../../Global.js" as Global

Item {
    id : inner
    width: 140
    x : 5
    y : 5
    height: parent.height -10
    property alias columnmodel : columnlist.model

    Text {
        id : title
        text : qsTr("Columns")
        width : parent.width
        x : 3
        font.weight: Font.DemiBold
    }

    ListView {
        id : columnlist
        width : parent.width - 3
        height :    parent.height - title.height
        anchors.top : title.bottom
        anchors.topMargin: 4
        x : 3
        clip : true
        highlight: attributeHighlight
        function iconsource(name) {
            if ( name.indexOf("/") !== -1)
                return name
            if ( name === "")
                name = "redbuttonr.png"

            var iconP = "../../images/" + name
            return iconP
        }
        Component {
            id: attributeHighlight

            Rectangle {
                width: columnlist.width - 6; height: 14
                color: Global.selectedColor; radius: 2
                y: (columnlist && columnlist.currentItem) ? columnlist.currentItem.y : 0
            }
        }
        delegate : Component {
            Item {
                width : 100
                height : 18
                Image{
                    id : domicon
                    source : iconsource(icon)
                    width : 16
                    height : 16
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    anchors.left: domicon.right
                    text : attributename
                    anchors.leftMargin: 4
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            columnlist.currentIndex = index
                            columnOperationList.currentColumn =columnlist.model[index]
                        }
                    }
                }
            }
        }
    }
}

