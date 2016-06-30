import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "Global.js" as Global

TabViewStyle {
    property string selectColor : Global.headerdark
    property string nonselectColor : Global.headerlight
    property int tabHeight : 22 * Global.uiScale
    property int minimumWidth : 100

    frameOverlap: 1
    tab: Rectangle {
        color: styleData.selected ? selectColor : nonselectColor
        border.color:  selectColor

        implicitWidth: Global.uiScale * Math.max(text.width + 4, minimumWidth)
        implicitHeight: tabHeight
        radius: 2
        Text {
            id: text
            anchors.centerIn: parent
            text: styleData.title
            color: styleData.selected ? "white" : "black"
            font.pointSize: 10 * Global.uiScale
        }
    }
    frame: Rectangle { color: "white"; border.width: 1 ; border.color: "lightgrey" }
}
