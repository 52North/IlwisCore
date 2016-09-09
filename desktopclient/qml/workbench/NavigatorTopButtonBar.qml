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
            width : parent.width
            Controls.ActionButtonV{
                id : addCatalog
                width : parent.width / 3
                height : buttonBar.height - 3
                iconsource: "../images/newcatalog_left.png"
                buttontext :  qsTr("New Left")

                function mouseClicked() {
                    bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "left")
                }

                Controls.ToolTip {
                    target: addCatalog
                    text: qsTr("Creates a new catalog in the left panel with the content\n of the last active catalog")
                }

            }
            Controls.ActionButtonV{
                id : addCatalog2
                width : parent.width / 3
                height : buttonBar.height - 3
                iconsource: "../images/newcatalog_right.png"
                buttontext :  qsTr("New Right")

                function mouseClicked() {
                    bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "right")
                }

                Controls.ToolTip {
                    target: addCatalog
                    text: qsTr("Creates a new catalog in the right panel with the content\n of the last active catalog")
                }


            }
            Controls.ActionButtonV{
                id : minimize
                width : parent.width / 3
                height : buttonBar.height - 3
                iconsource: createCatalogForm.state == "maximized" ? "../images/minimize_small_grey.png" : "../images/maximize_small_grey.png"
                buttontext :  createCatalogForm.state == "maximized" ? qsTr("Minimize") : qsTr("Maximize")

                function mouseClicked() {
                    createCatalogForm.state = (createCatalogForm.state == "maximized") ? "minimized" : "maximized"
                }

                Controls.ToolTip {
                    target: addCatalog
                    text: qsTr("Reduces the size of the navigator area")
                }


            }

        }

    }

}

