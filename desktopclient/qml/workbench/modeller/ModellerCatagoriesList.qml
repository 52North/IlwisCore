import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import OperationCatalogModel 1.0
import OperationsByKeyModel 1.0
import OperationModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global

Item {
    signal makeForm(string objectid, string name)

    id : operationCatTab
    anchors.fill: parent

    ListView{
        id : operationsByKey
        anchors.fill: parent
        model : operations.operationKeywords
        delegate: Controls.CollapsiblePanel {
            id : keylistdelegate
            width : parent.width
            titleText: keyword
            headerHeight: 20
            panelHeight: 35 * operationCount
            state : "collapsed"
            headerColor: Math.floor(index/5) % 2 == 0 ? Global.alternatecolor1 : Global.alternatecolor3


            ModellerList{
                id : operList
                operationsModel: operationsList
                width : parent.width
                height : parent.height
                parent : keylistdelegate.expandableArea
                byKey : true

                Connections{
                    target : operList
                    onMakeForm :{
                        makeForm(objectid,name)
                    }
                }
            }

            clip : true
        }
    }


}
