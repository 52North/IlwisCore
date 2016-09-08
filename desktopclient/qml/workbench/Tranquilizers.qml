import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import TranquilizerHandler 1.0
import "../Global.js" as Global

WorkBenchShifter {
    id : tranquilizerContainer
    property int defaultWidth: defaultFunctionBarWidth

    clip : true
    width : defaultWidth
    state : "invisible"
    height : 500

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/progress.png"
        headerText:"Progress"
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
                        y : 2
                        elide: Text.ElideMiddle
                    }

                }
                Item {
                    height : 10
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
                Text {
                    id : descText
                    x : 10
                    text : description
                    width : parent.width  -15
                    elide: Text.ElideMiddle
                    font.pointSize: 6
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

}
