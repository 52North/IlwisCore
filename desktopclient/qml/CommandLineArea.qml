import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import MasterCatalogModel 1.0
import "Global.js" as Global
import "controls" as Controls

Rectangle {
    id : commLine
    height : textArea.height
    color : Global.alternatecolor5
    Control.CommandLine{
        id : textArea
        y : 2
        anchors.left :parent.left
        anchors.leftMargin: 2
        anchors.right: parent.right
    }
}

