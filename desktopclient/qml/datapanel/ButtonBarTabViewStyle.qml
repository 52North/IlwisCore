import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../Global.js" as Global
import "../controls" as Controls

TabViewStyle {
    property string selectColor : "#B0C4DE"
    property string nonselectColor : "white"
    property int tabHeight : 23 * Global.uiScale


    frameOverlap: 1
    tab: Rectangle {
        id : labelArea
        color : styleData.selected ? selectColor :  nonselectColor
        border.color:  Global.edgecolor

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

