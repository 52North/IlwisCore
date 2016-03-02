import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.1
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0

import "../Global.js" as Global

Rectangle {

    signal makeForm(string objectid, string name, string url)

    property var operationsModel : operations.operations
    property bool byKey : false


    width : parent.width
    color : "white"
    opacity: width > 4 ? 1 : 0
    clip : true


    ListView {
        id : operationsList
        anchors.fill: parent
        model : operationsModel
        interactive : !byKey
        delegate : DOperation{}
    }

}
