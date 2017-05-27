import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import QtGraphicalEffects 1.0
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import MasterCatalogModel 1.0
import "Global.js" as Global
import "controls" as Controls
import "workbench" as WorkBench

Item {
    id : commLineRoot

    Row {
        id : commLine
        z: commLineRoot.z + 1
        width: parent.width
        height: parent.height
        Rectangle {
            id : aArea
            width : 80
            height : textArea.height
            color : uicontext.darkestColor
            Image {
                x : 4
                y : 4
                source : "images/ilwis_logo_txt.png"
            }
        }

        Control.CommandLine{
            id : textArea
            width : parent.width - aArea.width
            height : parent.height - 10
        }
    }
    DropShadow {
        id: cmlShadow
        z: commLine.z + 1
        anchors.fill: source
        cached: true
        verticalOffset: 4
        radius: 6
        samples: 12
        color: "#80000000"
        smooth: true
        source: commLine
    }

}

