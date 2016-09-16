import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick 2.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base
import ".." as DataPanel

Rectangle {
    id : columnManagement
    width: parent.width
    Layout.minimumHeight: 24
    function setColumnModel1(columnmodel){
        var tab = tabletabs.getTab(0)
        tab.active = true
        tab.item.setModel(columnmodel)
        tab = tabletabs.getTab(1)
        tab.active = true
        if ( tab)
            tab.item.columnmodel = columnmodel
    }



    TabView{
        anchors.fill: parent
        id : tabletabs
        tabPosition: Qt.BottomEdge

        function tabClicked(index){
            if ( currentIndex === index){
                if ( columnManagement.height <= 60){
                    tableView.state = "visible"
                }
                else{
                    tableView.state = ""
                    tableView.state = "invisible"
                }
            }

            currentIndex = index
        }
        Tab {
            title : qsTr("Operations")
            SplitView{
                id : columnview
                property string iconName : "../images/display_options"
                function setModel(cm){
                    //columnlist.columnmodel = cm
                }
                handleDelegate: Controls.SplitHandle{
                    imageHeight: 15
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
            active: true

            ColumnMetadataList {
                id : columnmetadatalist
            }
        }
        Tab {
            title : qsTr("Charts")
            id : graphpanel
            active : true
            ChartsPanel{

            }
        }

     style: DataPanel.ButtonBarTabViewStyle{}
    }
}

