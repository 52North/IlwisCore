import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../qml/controls" as Controls
import "../../../qml/Global.js" as Global


Item {
    id : createForm
    width : parent.width
    height : parent.height

    signal attributedefined(string attrname)

    Controls.TextEditLabelPair{
        id : attributenamefield
        labelWidth: 80
        labelText: qsTr("Attribute name")
        transparentBackgrond: false
    }
    Button {
        id : editAttrBut
        height : 22
        text : qsTr("Edit this attribute >>")
        anchors.top: attributenamefield.bottom
        onClicked: {
                // 1. Create attribute in Table
                // 2. Redirect to StandardizationTable.qml // HOW?
                console.log("attributenamefield.content:"+ attributenamefield.content)
                if (attributenamefield.content !== "")
                    attributedefined(attributenamefield.content)
                else console.log("attr not defined")
        }
    }
}
