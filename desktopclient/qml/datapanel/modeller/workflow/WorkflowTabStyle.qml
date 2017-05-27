import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls
import "../../.." as Base

TabViewStyle {
    property color backgroundColor: "white"
    property int tabHeight : 22

    frameOverlap: 1
    tab: Rectangle {
        id : labelArea
        implicitWidth: control.width / control.count
        implicitHeight: tabHeight
        BorderImage {
            id: name
            anchors.fill: parent
            border.left: 5; border.top: 5
            border.right: 5
            source : styleData.selected ? "../../../images/tab_big_active.png" : "../../../images/tab_big_inactive.png"


            Text {
                id: text
                anchors.centerIn: parent
                text: styleData.title
                color: styleData.selected ? "#003366" : "#a3a3c2"
                font.pointSize: 10
                font.weight: Font.DemiBold
            }
            Image {
                y : 3
                width : 16
                height : 16
                source : "../../../images/error_sign.png"
                visible: workflowView.workflow ? (styleData.index === 0 && !workflowView.workflow.isValid) : false
                anchors.right: name.right
                anchors.rightMargin: 4
            }


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

}

