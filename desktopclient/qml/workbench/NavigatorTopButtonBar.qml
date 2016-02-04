import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import "../controls" as Controls
import "../Global.js" as Global

Item {
    id : buttonBar

    Item {
        anchors.fill: parent
        x : 10
        Row {
            id : buttonRow
            anchors.top : parent.top
            anchors.margins: 3
            Controls.ActionButton{
                id : addCatalog
                width : 115
                height : buttonBar.height - 3
                iconsource: "../images/newcatalogCS1.png"
                buttontext :  qsTr("New Catalog Left")
                action : createCatalogLeft

            }
            Controls.ActionButton{
                id : addCatalog2
                width : 115
                height : buttonBar.height - 3
                iconsource: "../images/newcatalogCS1.png"
                buttontext :  qsTr("New Catalog Right")
                action : createCatalogRight

            }

        }

    }
    Row {
        height : 20
        anchors.right : parent.right
        anchors.rightMargin: -28
        anchors.top : parent.top
        anchors.topMargin: 4
        width : 75
        Action {
            id :maxButtons
            onTriggered: {
                createCatalogForm.state = "maximized"
            }
        }

        Action {
            id :minButtons
            onTriggered: {
                createCatalogForm.state = "minimized"
            }
        }
        Button{
            id : full
            height : 20
            width :20
            action : maxButtons
            Image { anchors.centerIn : parent; source: "../images/max1.png" }
        }
        Button{
            id : close
            height : 20
            width :20
            action : minButtons
            Image { anchors.centerIn : parent; source: "../images/min1.png" }
        }
    }
}

