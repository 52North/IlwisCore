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
    function setColumnModel1(columnmodel){
        var tab = tabletabs.getTab(0)
        tab.item.setModel(columnmodel)
        tab = tabletabs.getTab(1)
        tab.active = true
        if ( tab)
            tab.item.columnmodel = columnmodel
    }



    TabView{
        anchors.fill: parent
        id : tabletabs
        Tab {
            title : qsTr("Operations")
            SplitView{
                id : columnview
                function setModel(cm){
                    columnlist.columnmodel = cm
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
            title : qsTr("Statistics")
            id : columnmetadatview

            ColumnMetadataList {
                id : columnmetadatalist
            }
        }
        Tab {
            title : qsTr("Charts")
            id : graphpanel
            ChartsPanel{

            }
        }

        style: Base.TabStyle1{
        }
    }
}

