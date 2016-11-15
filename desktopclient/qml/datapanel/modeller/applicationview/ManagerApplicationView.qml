import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global


Item {
    id : appv
    width : parent.width
    height : parent.height
    property string iconName : "../images/app"

    SplitView {
        id : layerprops
        width : parent.width - 5
        height : parent.height

        function refreshAppNames(){
            firstColumn.refreshList();
        }

        y : 2
        ApplicationList {
            id : firstColumn
            width : 320
            height: parent.height
        }
        Item {
            id : rest
            width:parent.width - firstColumn.width
            Layout.fillWidth: true
            height : parent.height

        }
    }




}
