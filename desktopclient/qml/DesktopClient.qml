import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "workbench" as WorkBench
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import MasterCatalogModel 1.0
import QtGraphicalEffects 1.0
import PreferencesModel 1.0
import "Global.js" as Global
import "controls" as Controls

ApplicationWindow {
    id : bigthing
    width: Screen.width * 0.8
    height : Screen.height * 0.9
    color : uicontext.lightColor
    objectName: "mainwindow__mainui"

    onClosing: {
        uicontext.exitUI()
        rootwindow.close()
    }

    property int maxPull : 500
    property int defaultFunctionBarWidth : Math.min(bigthing.width * 0.25,390)
    property int activeCatalog: 0
    property double uiScale : preferences.uiScale

    function newCatalog(filter, outputtype, url, side){

        mainSplit.newCatalog(filter, outputtype, url, side)
    }

    function changeCatalog(filter, outputtype, url){
        mainSplit.changeCatalog(filter, outputtype, url)
    }

    function transitionInfoPane(newpagename, show) {
        mainSplit.transitionInfoPane(newpagename, show)
    }


    function getWorkbenchPane(name, vstate){
        mainSplit.getWorkbenchPane(name, vstate)
    }

    Controls.ToolTipLabel{id : toolTip}

    Column {
        id : root
        height : parent.height/uiScale
        width : parent.width/uiScale
        property int defaultCommLineHeight : 64

        scale : uiScale
        transformOrigin:Item.TopLeft

        Item {
            id : topArea
            width : parent.width
            height : root.defaultCommLineHeight



            Rectangle {
                anchors.left: parent.left
                anchors.top : parent.top
                width : xu1.width + 1
                height : 100
                color : uicontext.paleColor
                border.width: 1
                border.color : "#b3b3b3"
            }

            Item {
                id : xu1
                anchors.left: parent.left
                anchors.top : parent.top
                width : dummy.width + 7
                height : 100

                Rectangle{
                    id : dummy
                    height : 100
                    width : 70
                    anchors.left: parent.left
                    anchors.top : parent.top
                    color : uicontext.middleColor
                }
            }
            DropShadow {
                id: butShadow2
                anchors.fill: source
                cached: true
                horizontalOffset: 4
                radius: 6
                samples: 12
                color: "#80000000"
                smooth: true
                opacity: 0.7
                source: xu1
            }
            TopArea{
                width : parent.width
                height: parent.height
            }
        }


        MainSplit {
            id : mainSplit
            width: parent.width
            height : root.height - root.defaultCommLineHeight
            Layout.fillHeight: true
        }

    }
}
