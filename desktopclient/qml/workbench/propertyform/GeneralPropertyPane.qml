import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    id : container
    width : parent.width - 2
    height : parent.height -2
    anchors.margins: 2
    function storeData() {
        if ( propertyForm.editable){
            displayName = objectDisplayName.content
            description = descarea.text
            isReadonly = readOnlyStatus1.checked
            keywords = keys.content
            mastercatalog.currentCatalog.refresh()
        }
    }
    color : uicontext.lightestColor

    Column{
        id : grid
        x : 2
        height : 164
        width : parent.width - 6
        Controls.TextEditLabelPair{ id : objectDisplayName; labelText : qsTr("Display Name"); readOnly: !propertyForm.editable; labelWidth: 120; content : displayName; width : parent.width - 4}
        Controls.TextEditLabelPair{ labelText : qsTr("Logical Location"); readOnly: true; labelWidth: 120; content : url; width : parent.width - 4}
        Controls.TextEditLabelPair{ labelText : qsTr("Physical Location"); readOnly: true; labelWidth: 120; content : rawUrl; width : parent.width - 4}
        Controls.TextEditLabelPair{ labelText : qsTr("External format"); readOnly: true; labelWidth: 120; content : externalFormat; width : parent.width - 4}
        Controls.TextEditLabelPair{ labelText : qsTr("Creation date"); readOnly: true; labelWidth: 120; content : creationDate; width : parent.width - 4}
        Controls.TextEditLabelPair{ labelText : qsTr("Modified date"); readOnly: true; labelWidth: 120; content : modifiedDate; width : parent.width - 4}
        Controls.LabeledCheckBox{id : readOnlyStatus1; labelText : qsTr("Logical Read-only");checked : isReadonly; enabled: propertyForm.editable; labelWidth: 120}
        Controls.LabeledCheckBox{id : readOnlyStatus2; labelText : qsTr("Physical Read-only");checked : externalReadOnly; enabled: false; labelWidth: 120}
        Controls.TextEditLabelPair{ id : keys; labelText : qsTr("Keywords"); readOnly: !propertyForm.editable; labelWidth: 120; content : keywords; width : parent.width - 4}

        Item {
            width : parent.width
            height : 160
            Text { id : descriptionLabel; text : "Description"; font.bold: true; width : 120}
            TextArea{id : descarea

                height : 130
                anchors.left: descriptionLabel.right
                anchors.right: parent.right
                readOnly: !propertyForm.editable
                textFormat: TextEdit.AutoText

                Component.onCompleted: {
                    text = description
                }
            }
        }
    }
}
