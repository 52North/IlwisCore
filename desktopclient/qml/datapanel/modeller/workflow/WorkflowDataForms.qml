import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../.." as Base

TabView {
   // anchors.fill: parent

    onCurrentIndexChanged: {
        if ( currentIndex == 0)
            updateRunForm()
        if ( currentIndex == 2)
            updateMetadata()
    }

    Tab{
        id : runTab
        title: qsTr("Run form")
        WorkflowRunForm{
            id : runForm
        }
    }
    Tab{
        id : selectedTab
        title: qsTr("Selected operation Form")
        SelectedOperationForm{
        }
    }
    Tab{
        id : metaTab
        title: qsTr("Metadata")
        SelectedOperationMetaData{}
    }
    Tab{
        title: qsTr("Step Form")
    }



    style: Base.TabStyle1 {
        id : tabstyle
    }

    function updateRunForm() {
        runTab.active = true
        runTab.item.fillAppFrame()
    }
    function executeRunForm(runparms){
        runTab.active = true
        runTab.item.executeRunForm(runparms)
    }

    function updateMetadata() {
        metaTab.active = true
        metaTab.item.selectedOperation()
    }

    function showSelectedOperation(opIndex){
        if ( opIndex){
            selectedTab.active = true
            metaTab.active = true
            if ( currentIndex != 2)
                currentIndex = 1
            selectedTab.item.fillAppFrame()
            metaTab.item.selectedOperation()
        }else
            currentIndex = 0
    }

}
