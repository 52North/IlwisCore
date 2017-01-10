import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import Qt.labs.folderlistmodel 2.1
import "../../../Global.js" as Global
import "../../../controls" as Controls
import OperationModel 1.0

Rectangle {
    width: 350
    height: 0
    enabled: false
    anchors.centerIn: parent
    color : "white"
    state : "invisible"
    border.width: 1
    border.color: Global.edgecolor
    radius : 4
    z : ontTopZValue
    clip : true
}

