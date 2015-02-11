import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

TabViewStyle {
    property string selectColor : "steelblue"
    property string nonselectColor : "lightsteelblue"
    property int tabHeight : 22

    frameOverlap: 1
    tab: Rectangle {
        id : selectRect
        color: indexTab === styleData.index && activeSplit === side ? selectColor : nonselectColor
        border.color:  color

        implicitWidth: Math.max(label.width + 4, 140)
        implicitHeight: tabHeight
        radius: 2
        Text {
            id: label
            width : 100
            anchors.centerIn: parent
            text: styleData.title
            color: styleData.selected ? "white" : "black"
            elide: Text.ElideMiddle
        }
        MouseArea  {
            width : label.width
            height : parent.height
            onClicked : {
                indexTab = styleData.index
                mastercatalog.setActiveTab(Math.abs(side) - 1, indexTab)
                side = -side
            }
        }

        Button {
            y : 2
            anchors.right : parent.right
            anchors.rightMargin: 2
            width : 18
            height : 18
            opacity : 0.5
            onClicked: {
               mainsplit.closeTab(side, styleData.index)

            }

            Image {
                y : 2
                x : 1
                width : 15
                height : 15
                source : "images/close20.png"


            }
        }

    }
    frame: Rectangle { color: "white"; border.width: 1 ; border.color: "lightgrey" }
}
