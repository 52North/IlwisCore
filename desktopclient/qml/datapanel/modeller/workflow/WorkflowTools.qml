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
            source : "../GenericTools.qml"
            height : buttonSize
            onLoaded: {
                width = item.width
            }
        }

        Controls.ToolButton{
            height : buttonSize
            width : buttonSize
            iconSource: iconsource("saveb.png")
            tooltip: qsTr("Saves the workflow in the current active catalog(if possible)")

            onClicked: {
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
            onClicked: {
                workarea.dropSaveBox(x)
            }
        }




        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            iconSource : iconsource("trash20.png")
            tooltip: qsTr("Removes a selected flow or operation)")
            onClicked: {
                workarea.deleteSelectedItem()

            }
        }
         Controls.ToolButton {
             height : buttonSize
             width : buttonSize
            iconSource : iconsource("run20.png")
            onClicked: {
                workflowManager.executeRunForm({"runid" :  workflow.id, "stepmode" :false})
            }
        }
        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            id : stepButton
            opacity : enabled ? 1 : 0.5
            iconSource : iconsource("step20.png")
            onClicked :{
                //  if ( stepModeCheck.checked )
                //      modellerDataPane.workflowModel().nextStep()
                //     modellerDataPane.nextStep()
            }
        }
        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            iconSource : iconsource("stop20.png")
        }

        Controls.ToolButton {
            height : buttonSize
            width : buttonSize
            id : newcondition
            checked: false
            checkable: true
            exclusiveGroup: toolgroup
            iconSource: iconsource("choice20.png")
            onClicked: {
                workarea.dropCondition = checked
            }
        }

//        Controls.ToolButton {
//            height : buttonSize
//            width : buttonSize
//            id : loop
//            checked: false
//            checkable: true
//            exclusiveGroup: toolgroup
//            iconSource: iconsource("loop.png")
//        }


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

