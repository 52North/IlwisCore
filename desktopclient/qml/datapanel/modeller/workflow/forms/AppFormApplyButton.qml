import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../../workbench" as Bench
import "../../../../Global.js" as Global

Button {
    height : saveButtonEnabled ? 25 : 0
    width : 70
    Row {
        anchors.fill: parent
        anchors.leftMargin: 4
        spacing : 4
        visible: saveButtonEnabled
        Image {
            anchors.verticalCenter: parent.verticalCenter
            source : "../../../../images/apply20.png"
        }
        Text {
            height : parent.height
            id : saveConstantInputText
            text: saveButtonEnabled ? 'Apply' : ""
            y : 5
        }
    }

    onClicked: {
        asignConstantInputData()
    }
    Component.onCompleted: {
        console.debug(saveButtonEnabled)
    }
}
