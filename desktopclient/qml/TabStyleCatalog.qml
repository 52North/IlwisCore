import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "Global.js" as Global
import "controls" as Controls

TabViewStyle {
    property color backgroundColor: "white"
    property int tabHeight : 50 * Global.uiScale
    property int minimumWidth : 100

    frameOverlap: 1
    tab: BorderImage {
        source: control.getTab(styleData.index).iconName + (styleData.selected ?"_active.png" : "_inactive.png")
        border.left: 10; border.top: 10
        border.right: 10;
        id : labelArea

        width: Global.uiScale * Math.max(text.width + 4, minimumWidth)
        height: tabHeight

//        Image {
//            source : control.getTab(styleData.index).iconName
//            anchors.top: parent.top
//            anchors.topMargin: 1
//            anchors.horizontalCenter: parent.horizontalCenter
//        }

        Text {
            id: text
            anchors.bottom : parent.bottom
            anchors.bottomMargin: 2
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment:Text.AlignHCenter

            text: styleData.title
            color: styleData.selected ? "#003366" : "#a3a3c2"
            font.pointSize: 10 * Global.uiScale
            font.weight: Font.DemiBold
        }

        Controls.ToolTip{
            id : ttip
            target : labelArea
        }

        Component.onCompleted: {
            if ( 'tooltips' in control ){
                ttip.text = control.tooltips[styleData.index]
            }
        }
    }
    frame: Rectangle { color: "white"; border.width: 1 ; border.color: "lightgrey" }


}
