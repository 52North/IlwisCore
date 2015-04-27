import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick 2.0
import "../../Global.js" as Global
import "../.." as Base

Rectangle {
    id : columnManagement
    width: parent.width
    height: 250
    Layout.minimumHeight: 22
    function setColumnModel(columnmodel){
        var tab = tabletabs.getTab(0)
        tab.item.cmodel = columnmodel
    }

    TabView{
        anchors.fill: parent
        id : tabletabs
        Tab {
            title : qsTr("Operations")
            SplitView{
                id : columnview
                property var cmodel

                onCmodelChanged: {
                    columnlist.columnmodel = cmodel
                }

                ColumnList {
                    id : columnlist
                }
                ColumnOperationList{
                    id : columnOperationList
                }
                ColumnOperation{
                    id : columnOperation
                    Layout.fillWidth: true
                }
            }

        }

        Tab {
            title : qsTr("Metadata")
//            SplitView{
//                id : columnmetadatview
//                property var cmodel2

//                onCmodel2Changed: {
//                    columnnamelist.columnmodel = cmodel2
//                }

//                ColumnList {
//                    id : columnnamelist
//                }
//                MetaDataPane{
//                    id : columnmetadata
//                    Layout.fillWidth: true
//                }
//            }
        }
        Tab {
            title : qsTr("Charts")
        }

        style: Base.TabStyle1{
        }
    }
}

