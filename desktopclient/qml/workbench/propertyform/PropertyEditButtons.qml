import QtQuick 2.0
import QtQuick.Controls 1.0

Row {
    Button {
        id : editBut
        text : qsTr("Edit")
        checkable: true
        checked : false
        onClicked: {
            applyBut.enabled = !applyBut.enabled
            checked = !checked
            propertyForm.editable = !propertyForm.editable
            text = (text === qsTr("Edit")) ? qsTr("Cancel") : qsTr("Edit")
        }
    }
    Button {
        id : applyBut
        text : qsTr("Apply")
        enabled: false
        onClicked: {
            if ( enabled && propertyForm.editable){
                propertyForm.storeData()
                enabled = false
            }

        }
    }
}

