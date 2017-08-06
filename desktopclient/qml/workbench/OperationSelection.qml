import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import ".." as Base
import "../Global.js" as Global
import "../controls" as Controls

WorkBenchShifter {
    id : operationSelectionContainer

    property var currentAppForm : null
    property var operationid

    clip : true
    state : "invisible"

    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/operations.png"
        headerText:qsTr("Operations")
    }

    OperationSearchBar {
        id : searchBar
        height : 48
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : functionBarHeader.x
    }
    SplitView{
        width : parent.width
        anchors.bottom: operationSelectionContainer.bottom
        anchors.top : searchBar.bottom;
        anchors.topMargin: 5
        orientation: Qt.Vertical
        OperationFormArea {
            id : applicationForm
            width : parent.width
            x : parent.x + 5
        }
        TabView {

            id : operationTabs
            width : parent.width

            Tab {
                title : qsTr("Operation List")
                OperationList {
                    id : operationList
                    Connections {
                        target : operationList
                        onMakeForm : {
                            applicationForm.newForm(objectid, name)
                        }
                    }
                }


            }
            Tab {
                title : qsTr("Operation By Keyword")
                OperationCatagoriesList{
                    id : operationCatagories
                    Connections {
                        target : operationCatagories
                        onMakeForm : {
                            applicationForm.newForm(objectid, name)
                        }
                    }
                }

            }
            style: Base.TabStyle1{}
        }
    }

}
