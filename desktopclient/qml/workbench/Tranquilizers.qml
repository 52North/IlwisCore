import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import TranquilizerHandler 1.0

Rectangle {
    id : tranquilizerContainer
    property int defaultWidth: defaultFunctionBarWidth

    signal unloadcontent(string content)

    color : background4
    clip : true
    width : defaultWidth
    state : "invisible"
    height : 500
    opacity : 1

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/progress40.png"
        headerText:"Tranquilizers"
    }

    Component{
        id : tranquilizerDelegate
        Item {
            width: parent.width
            height : 40
            ProgressBar{
                id : progressbar
                minimumValue: 0
                maximumValue: 100
                value : currentValue
            }
        }
    }

    ListView{
        id : tranquilizerList
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        model : tranquilizerHandler.tranquilizers
        delegate: tranquilizerDelegate
    }





    states: [
        State { name: "visible"

            PropertyChanges {
                target: tranquilizerContainer
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: tranquilizerContainer
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity"; duration : 500 ; easing.type: Easing.Linear
            }
            onRunningChanged :
            {
                if ( opacity == 0) {
                    unloadcontent("Tranquilizers.qml")
                }
            }

        }
    ]
}
