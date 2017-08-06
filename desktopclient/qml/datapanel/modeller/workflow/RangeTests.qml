import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global
import "../../../controls" as Controls

Rectangle {
    id: testRectangle

    border.width: 1
    border.color: "grey"
    color: "grey"//workflow.isValidNode(itemid,"tests") ? testColor : Global.errorColor
    height : 35
    width : parent.width - 8
    x : 4
    y: 4
    radius : 5
    z : 10

    DropArea {
        x : 2
        y : 2
        z: 1

        height : parent.height
        width : parent.width - 4
        enabled : true

        Text {
           anchors.centerIn: parent
           text : qsTr("Drop test operation(s) here")
           color : "grey"
        }
        onDropped: {
            addTestOperation(drag.source.ilwisobjectid)
        }

        TextField {
            id : testArea
            width : parent.width - 28
            height : parent.height - 6
            y : 1
            z : 2
            readOnly: true
        }

        Rectangle {
            width : 22
            height : 22
            anchors.left: testArea.right
            anchors.leftMargin: 3
            y : 2
            Image{
                width : 20
                height : 20
                source : icon
                anchors.centerIn: parent
            }
        }
    }



    function addTestOperation(objectid){

    }
}
