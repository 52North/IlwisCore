import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../qml/controls" as Controls
import "../../../qml/Global.js" as Global


Item {
    id : nodeForm
    width : parent.width
    height : parent.height


    Controls.ComboxLabelPair{
        id : nodeType
        width : parent.width
        height : 50
        labelWidth: 100
        labelText: qsTr("Node type")
        itemModel: {}
    }


    Controls.TextEditLabelPair{
        id : nodeText
        labelWidth: 80
        labelText: qsTr("Text")
        transparentBackgrond: false
    }



    Controls.LabeledCheckBox {
        id: ro
    }

    Button {
        id : applyBut
        height : 22
        text : qsTr("Apply")
        anchors.top: nodeText.bottom
        onClicked: {

        }
    }
}
