import QtQuick 2.0
import "../../Global.js" as Global
Rectangle {
    height : 45
    width : parent.width - 10
    color : index % 2 ? "white" : Global.alternatecolor4
    property string parmnam
    property string parmtype
    property string parmdesc
    Column{
        anchors.fill: parent
        Row {
            width : parent.width
            height : 15
            Text{
                id : namelabel
                width : 50
                height : 15
                text : qsTr("Name")
                font.pointSize: 8
                font.bold : true
            }

            Text{
                text : parmnam
                font.pointSize: 8
                height : 15
                width : parent.width - namelabel.width - 15
            }
        }
        Row {
            width : parent.width
            height : 15
            Text{
                id : typeLabel
                width : 50
                height : 15
                text : qsTr("Type")
                font.pointSize: 8
                font.bold : true
            }

            Text{
                text : parmtype
                font.pointSize: 8
                height : 15
                width : parent.width - typeLabel.width - 15
            }
        }
        Row {
            width : parent.width
            height : 15
            Text{
                id : descLabel
                width : 50
                height : 15
                text : qsTr("Descr.")
                font.pointSize: 8
                font.bold : true
            }

            Text{
                text : parmdesc
                font.pointSize: 8
                height : 15
                width : parent.width - descLabel.width - 15
                elide: Text.ElideMiddle
            }
        }
    }

}

