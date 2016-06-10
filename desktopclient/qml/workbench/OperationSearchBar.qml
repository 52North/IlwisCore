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

Rectangle {
    id : searchBar
    color : Global.alternatecolor1

    Column {
        y : 2
        anchors.fill: parent
        spacing : 3
        Controls.TextEditLabelPair{

            labelText:  qsTr("Filter Text")
            labelWidth: 100
            width : parent.width - 9
            transparentBackgrond: false
            onContentChanged: {
                operations.nameFilter = content
                operations.refresh()
            }
        }
        Row {
            height : 22
            width : parent.width
            Controls.TextEditLabelPair{
                id : filtertxt
                labelText:  qsTr("Keyword filter")
                labelWidth: 100
                width : parent.width - 49
                transparentBackgrond: false
                onContentChanged: {
                    operations.keyFilter = filtertxt.content;
                    operations.refresh()
                }
            }
//            Button{
//                width : 40
//                height : 20
//                text : qsTr("Apply")
//                onClicked: {
//                    operations.keyFilter = filtertxt.content;
//                }
//            }
        }
    }
}

