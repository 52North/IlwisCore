import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import ".." as DataPanel

Rectangle {
    id : modelmanager
    width: parent.width
    height: 62


    function showMetaData(item){
        operationMetaData.item.setDesc(item.description)
        operationMetaData.item.setName(item.syntax)
        operationMetaData.item.setKeywords(item.keywords)
    }

    function resetMetaData(item){
        operationMetaData.item.setDesc("")
        operationMetaData.item.setName("")
        operationMetaData.item.setKeywords("")
    }

    function showWorkflowMetaData(workflow){
        workflowMetaData.item.setDesc(workflow.description)
        workflowMetaData.item.setName(workflow.name)
        workflowMetaData.item.setKeywords(workflow.keywords)
    }

    TabView{
        id : formTabs
        anchors.fill: parent
        tabPosition: Qt.BottomEdge

        function tabClicked(index){
            if ( currentIndex === index){
                if ( modelmanager.height <= 60){
                    datapane.state = "smaller"
                }
                else{
                    datapane.state = "bigger"
                }
            }

            currentIndex = index
        }


        Tab {
            id : operationMetaData
            active : true
            title: qsTr("Operation metadata")
            MetaData{}
        }

        Tab {
            id : workflowMetaData
            title: qsTr("Workflow metadata")
            active: true
            MetaData{}
        }

        style: DataPanel.ButtonBarTabViewStyle{
            selectColor: Global.alternateSelectedTabColor
        }
    }



}

