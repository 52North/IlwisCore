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
        color: styleData.selected ? selectColor : nonselectColor
        border.color:  selectColor

        implicitWidth: Math.max(text.width + 4, 100)
        implicitHeight: tabHeight
        radius: 2
        Text {
            id: text
            anchors.centerIn: parent
            text: styleData.title
            color: styleData.selected ? "white" : "black"
        }
    }
    frame: Rectangle { color: "#C6C6C6"; radius : 10 }
}
