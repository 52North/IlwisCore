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

ScrollView{
    id: operationFormScrollView
    property var container
    width : parent.width
    property alias appForm : appFrame

    Bench.ApplicationForm{
        id : appFrame
        width : operationFormScrollView.width - 20
        height : container.height - 25 < 0 ?  0 : container.height - 25
        opacity : 0
        showTitle: false

    }
}
