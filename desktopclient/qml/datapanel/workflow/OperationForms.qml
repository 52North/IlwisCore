import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../workbench" as Bench
import "../../Global.js" as Global

Rectangle {
    id : operationForm
    width : 210
    height : parent.height
    property var operationid

    function newForm(metaid, title){
        operationid = metaid
        var form= formbuilder.index2Form(metaid, false)
        appFrame.formQML = form
        appFrame.formTitle = title
        appFrame.opacity = 1
    }


    Bench.ApplicationForm{
        id : appFrame
        width : parent.width / 2
        height : parent.height - 30 < 0 ?  0 : parent.height - 30
        opacity : 0

    }
}

