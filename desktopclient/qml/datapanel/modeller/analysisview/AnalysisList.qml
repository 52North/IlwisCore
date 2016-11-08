import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Item {
    id : firstColumn
    width : 320
    height: parent.height
    Column {
        width : parent.width - 8
        x : 4
        height : parent.height - 8
        y : 4
        spacing: 4
        Row {
            width : parent.width
            height : Global.rowHeight
            Controls.ComboxLabelPair{
                id : analysisList
                width : parent.width - createBut.width
                height : parent.height
                labelWidth: 100
                labelText: qsTr("Analysis type")
            }
            Button{
                id : createBut
                width :70
                height:Global.rowHeight
                Image{
                    anchors.centerIn : parent;
                    width : Global.rowHeight - 6;
                    height:Global.rowHeight - 6;
                    source: "../../../images/plus20.png"
                    fillMode: Image.PreserveAspectFit
                }

                onClicked : {
                    createForm.state = "visible"
                }
            }
        }
        Rectangle {
            width : parent.width
            height : parent.height - Global.rowHeight - 10
            border.width: 1
            border.color: Global.edgecolor
            ListView {
                anchors.fill: parent
                anchors.margins: 3
            }
        }
    }

}
