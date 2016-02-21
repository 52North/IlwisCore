import QtQuick 2.2
import "../../Global.js" as Global

Rectangle{
    id : labelrect
    width : parent.width
    height : 22
    color : Global.headerdark
    Text{
        text : creator.label
        color: "white"
        x : 4
        font.pointSize: 9
        font.bold: true
        anchors.verticalCenter: parent.verticalCenter
    }

}


