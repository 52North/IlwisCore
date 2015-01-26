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


    frameOverlap: 1
    tab: Rectangle {
        color: styleData.selected && activeSplit === Math.abs(splitindex) ? selectColor : nonselectColor
        border.color:  color

        implicitWidth: Math.max(text.width + 4, 100)
        implicitHeight: tabHeight
        radius: 2
        Text {
            id: text
            anchors.centerIn: parent
            text: styleData.title
            color: styleData.selected ? "white" : "black"
        }
        MouseArea  {
            anchors.fill: parent
            onClicked : {
                splitindex = -splitindex
                indexTab = styleData.index

            }
        }
    }
    frame: Rectangle { color: "white"; border.width: 1 ; border.color: "lightgrey" }
}
