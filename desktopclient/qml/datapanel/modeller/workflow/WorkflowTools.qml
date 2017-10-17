import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../../controls" as Controls

ToolBar{
    id : workflowtools
    width : parent.width
    height : 45

    property int buttonSize : 30


    Row {
        anchors.verticalCenter: parent.verticalCenter

        width : childrenRect.width
        height : buttonSize
        spacing : 3
        id : editingtools

        Loader {
            id : butLoader
            source : workflowOnly ? "" : "../GenericTools.qml"
            height : buttonSize
            width : workflowOnly ? 0 : 60
        }

        Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("saveb.png")
            tooltip: qsTr("Saves the workflow in the current active catalog(if possible)")

            onClicked: {
                workflowView.storeRangeDefinitions()
                var url = mastercatalog.currentCatalog.url
                if ( url.indexOf("file://") !== 0) {
                    url = workflow.rawUrl
                    if(url.indexOf("file://") !== 0)
                        return
                }
                workflow.store("","")
            }
        }
        Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("saveasb.png")
            tooltip: qsTr("Copies the workflow in the current active catalog under a different name(if possible)")
            onClicked: {
                workflowView.storeRangeDefinitions()
                workarea.dropSaveBox(x)
            }
        }

        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            iconSource : iconsource("trash20.png")
            tooltip: qsTr("Removes a selected flow or operation")
            onClicked: {
                workarea.deleteSelectedItem()

            }
        }
         Controls.ToolButton {
             height : buttonSize
             width : buttonSize
             enabled : workflow ? workflow.isValid : false
             opacity : enabled ? 1 : 0.2
            iconSource : iconsource("run20.png")
            tooltip: qsTr("Starts a workflow from the first input node")
            onClicked: {
                workflowManager.executeRunForm({"id" :  workflow.id, "runid" : workflow.runid, "stepmode" :false})
            }
        }
        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            id : stepButton
            enabled : workflow ? workflow.isValid : false
            opacity : enabled ? 1 : 0.2
            iconSource : iconsource("step20.png")
            tooltip: qsTr("Starts a workflow and stops after each operation has run. Pressing the button again moves it to the next operation")
            onClicked :{
                var wasInStepMode = workarea.workflow.stepMode
                workarea.workflow.stepMode = true
                if ( !wasInStepMode){ // start the workflow; after this
                    workflow.resetStepMode()
                    var ret = workflowManager.executeRunForm({"id" :  workflow.id, "runid" : workflow.runid, "stepmode" :true})
                    if ( ret === "?"){
                       workarea.workflow.stepMode = false
                    }
                }else {
                    workarea.workflow.nextStep()
                }
            }
        }
        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            iconSource : iconsource("stop20.png")
            tooltip: qsTr("Ends the workflow in either step mode or normal mode")
            enabled : workflow ? workflow.isValid : false
            opacity : enabled ? 1 : 0.2
            onClicked: {
                workarea.workflow.stopExecution()
            }
        }

        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            id : newcondition
            checked: false
            checkable: true
            exclusiveGroup: toolgroup
            iconSource: iconsource("choice20.png")
            tooltip : qsTr("When activated the next click on the workflow panel will create a new condition item")
            onClicked: {
                workarea.dropCondition = checked
            }
        }
        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            id : loop
            checked: false
            checkable: true
            exclusiveGroup: toolgroup
            iconSource: iconsource("loop.png")
            onClicked: {
                workarea.dropLoop = checked
            }
        }

        Controls.ToolButton {
            id : colbut
            property bool collapsed: false
            height : buttonSize
            width : buttonSize
            checkable: true
            exclusiveGroup: toolgroup
            iconSource: iconsource("collapse.png")
            tooltip : qsTr("Collapses all operation to a minimized for or vice versa")
            onClicked: {
                colbut.collapsed = !colbut.collapsed
                workarea.collapse(colbut.collapsed)
            }
        }

        ExclusiveGroup {
            id: toolgroup
        }

    }

    Row{
        x : 3
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        height : buttonSize
        width: childrenRect.width
        spacing : 2

        PanButton{
            height : buttonSize
            width : buttonSize
            direction : "up"
        }
        PanButton{
            height : buttonSize
            width : buttonSize
            direction : "left"
        }
        PanButton{
            height : buttonSize
            width : buttonSize
            direction : "right"
        }
        PanButton{
            height : buttonSize
            width : buttonSize
            direction : "down"
        }

        Button {
            height : buttonSize
            width : buttonSize
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource("zoomin20.png")
            }
            onClicked: {
                workarea.zoom(3,false,-1,-1)
            }
        }

        ComboBox {
            id : zoomFactor
            editable: true
            height : buttonSize
            width : 75
              model : ["10%","25%","50%","70%","80%","90%","100%","125%","150%", "200%"]

            onCurrentIndexChanged: {
                if ( currentText == "")
                    return;
                var perc = Number(currentText.slice(0, -1))
                zoom(perc, true,-1,-1)
            }

            Component.onCompleted: {
                setZoomEdit(100 + "%")
            }

        }

        Button {
            height : buttonSize
            width : buttonSize
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource("zoomout20.png")
            }
            onClicked: {
                workarea.zoom(-3,false,-1,-1)
            }
        }

    }
    function dropCondition(yesno){
       newcondition.checked = yesno
    }

    function dropLoop(yesno){
       //loop.checked = yesno
    }


    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../../images/" + name
        return iconP
    }

    function setZoomEdit(amount){
        zoomFactor.editText = amount
    }



}

