import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../Global.js" as Global
import "../controls" as Controls

TabViewStyle {
    property string selectColor : "white"
    property string nonselectColor : "#e6e6e6"
    property int tabHeight : 23 * Global.uiScale


    frameOverlap: 1
    tab: Rectangle {
        id : labelArea
        color : styleData.selected ? selectColor :  nonselectColor
        border.color:  selectColor

        implicitWidth: control.width / control.count
        implicitHeight: tabHeight

        Controls.ToolTip{
            id : ttip
            target : labelArea
            up : true
        }

        Text {
            id: text
            anchors.centerIn: parent
            text: styleData.title
            font.pointSize: 9 * Global.uiScale
            font.weight: Font.DemiBold
            color: styleData.selected ? "#003366" : "#a3a3c2"
            anchors.verticalCenter: parent.verticalCenter
        }
        MouseArea  {
            anchors.fill : parent
            onClicked: {
                control.tabClicked(styleData.index)
            }
        }

        Component.onCompleted: {
            if ( 'tooltips' in control ){
                ttip.text = control.tooltips[styleData.index]
            }
        }
    }
    frame: Rectangle { color: Global.alternatecolor3; border.width: 1 ; border.color: "lightgrey" }
}

