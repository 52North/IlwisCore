import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../Global.js" as Global

Item {
    property alias checked : cb.checked
    property alias labelText : label.text
    property int labelWidth : 100
    property alias style : cb.style
    property alias exclusiveGroup : cb.exclusiveGroup
    property alias pressed : cb.pressed

    height : Global.rowHeight
    width : parent.width

    Text { id : label; text : qsTr("Read-only"); font.bold: true}
    CheckBox { id : cb;  x : labelWidth}
}

