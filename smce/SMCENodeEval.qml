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


    Controls.TextEditLabelPair{
        id : nodeText
        labelWidth: 80
        labelText: qsTr("Input indicator")
        transparentBackgrond: false
    }

    Controls.TextEditLabelPair{
        id : weight
        labelWidth: 80
        labelText: qsTr("Weight")
        transparentBackgrond: false
        anchors.top: nodeText.bottom
    }

    Button {
        id : applyBut
        height : 22
        text : qsTr("Apply")
        anchors.top: weight.bottom
        onClicked: {

        }
    }
}
