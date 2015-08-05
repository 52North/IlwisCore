import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import RepresentationElement 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Item {
    id : legend
    width: parent ? parent.width : 0
    height: parent ? parent.height : 0
    property var editor

    SplitView{
        anchors.fill: parent
        orientation: Qt.Vertical
        LegendGrid{
            id : legendGrid
            height : parent.height
        }

        LegendItemEditor{

            id : itemEditor
        }

    }
    states: [
        State { name: "visible"

            PropertyChanges {
                target: layers
                state : "bigger"
            }
            PropertyChanges {
                target: legendGrid
                height : 140
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: layers
                state : "visible"
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
        }
    ]

}

