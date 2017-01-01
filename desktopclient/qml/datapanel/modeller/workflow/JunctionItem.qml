import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import OperationModel 1.0
import "../../../Global.js" as Global

Rectangle {
    id : junctionItem
    property string type : "junctionitem"
    property var itemid;
    property bool isSelected : false

    width : 39
    height : 30
    radius : 30
    color : Global.lightgreen
    border.width: 1
    border.color: Global.darkgreen

    AttachmentRectangle{
        id : att1
        index : 0
        anchors.top: junctionItem.top
        anchors.topMargin: -3
        anchors.left : junctionItem.left
        owner : junctionItem
    }
    AttachmentRectangle{
        id : att2
        index : 1
        anchors.top: junctionItem.top
        anchors.topMargin: -3
        anchors.left : junctionItem.right
        owner : junctionItem
    }
    AttachmentRectangle{
        id : att3
        index : 2
        anchors.top: junctionItem.bottom
        anchors.topMargin: -3
        anchors.left : junctionItem.left
        owner : junctionItem
    }

    AttachmentRectangle{
        id : att4
        index : 3
        anchors.bottom: junctionItem.bottom
        anchors.bottomMargin: -3
        anchors.left : junctionItem.right
        owner : junctionItem
    }

    function attachFlow(target, attachRect){

    }

    function center(){
        return Qt.point(x + width/2, y + height/2)
    }

    function deselectAll(){
        att1.isSelected = att2.isSelected = att3.isSelected = att4.isSelected = false
    }


}
