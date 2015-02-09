import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

TabViewStyle {
    property string selectColor : "steelblue"
    property string nonselectColor : "lightsteelblue"
    property int tabHeight : 22
    property int splitindex  : 0
    property int indexTab : 0


    Action {
        id : closeStuff
        onTriggered: {
            mainsplit.closeTab(splitindex, indexTab)
        }
    }

    frameOverlap: 1
    tab: Rectangle {
        color: styleData.selected && activeSplit === Math.abs(splitindex) ? selectColor : nonselectColor
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
                mastercatalog.setActiveTab(Math.abs(splitindex) - 1, indexTab)
                splitindex = -splitindex

            }
        }

        Button {
            y : 2
            anchors.right : parent.right
            anchors.rightMargin: 2
            width : 18
            height : 18
            opacity : 0.5
            action : closeStuff
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
