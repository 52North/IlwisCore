import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

Rectangle {
    id : container
    anchors.fill : parent
    GridLayout{
        id : grid
        height : 144
        width : parent.width
        columns : 2
        Text { text : qsTr("Location"); font.bold: true }
        TextField { text : url; readOnly : true; Layout.fillWidth: true }
        Text { text :  qsTr("External format"); font.bold: true }
        Text { text : externalFormat}
        Text { text : qsTr("Creation date") ; font.bold: true}
        Text { text :  creationDate }
        Text { text : qsTr("Modified date"); font.bold: true}
        Text { text : modifiedDate }
        Text { text : qsTr("Read-only"); font.bold: true}
        CheckBox { checked: isReadonly}
        Text { text : qsTr("Target read-only"); font.bold: true}
        CheckBox { checked: externalReadOnly; enabled : false}
        Text { text : qsTr("Size in bytes"); font.bold: true}
        Text { text :size }
    }
    Item {
        anchors.top: grid.bottom; anchors.topMargin: 4
        Text { id : descriptionLabel; text : "Description"; font.bold: true}
        TextArea{text : description ;width : container.width - descriptionLabel.contentWidth - 34; height : 60;anchors.left: descriptionLabel.right; anchors.leftMargin: 28}
    }

}
