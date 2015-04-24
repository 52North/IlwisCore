import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import TranquilizerHandler 1.0
import "../Global.js" as Global

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
        Rectangle {
            width: parent.width
            height : 50
            color : index % 2  ? Global.mainbackgroundcolor : Global.alternatecolor4
            Column {
                width : parent.width
                height : 43
                anchors.verticalCenter: parent.verticalCenter
                Item {
                    id : topline
                    height : 20
                    width : parent.width
                    ProgressBar{
                        id : progressbar
                        x : 10
                        minimumValue: startValue
                        maximumValue: endValue
                        value : currentValue
                        width : 160
                        height : 20
                    }
                    Text {
                        id : titleText
                        anchors.left: progressbar.right
                        anchors.leftMargin: 5
                        text : title
                        width : 150
                        y : description !== "" ? 0 : 2
                        elide: Text.ElideMiddle
                    }
                    Text {
                        id : descText
                        anchors.left: titleText.left
                        anchors.leftMargin: 5
                        anchors.top : titleText.bottom
                        text : description
                        width : 150
                        elide: Text.ElideMiddle
                        font.pointSize: 6
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                Item {
                    height : 20
                    width : parent.width
                    Text{
                        x : progressbar.x - contentWidth / 2
                        text : startValue
                        font.pointSize: 6
                    }
                    Text {
                        x : progressbar.x + progressbar.width - contentWidth / 2
                        text : endValue
                        font.pointSize: 6
                    }
                }
            }
        }
    }

    ListView{
        id : tranquilizerList
        anchors.top : functionBarHeader.bottom
        height : 300
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
