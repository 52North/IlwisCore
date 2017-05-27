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
import "Global.js" as Global
import "controls" as Controls


Item {

    id : topRoot
    Item {
        id : topItem
        z: topRoot.z + 1
        width: parent.width
        height: parent.height - 6
        Rectangle {
            id : aArea
            width : parent.width
            height : parent.height - 6
            color : uicontext.darkestColor
            Image {
                width : 54
                anchors.verticalCenter: parent.verticalCenter
                source : "images/ilwis_logo_txt.png"
                x : 5
            }

            Row {
                id : buttonRow
                width : childrenRect.width
                height : parent.height - 2

                anchors.verticalCenter: parent.verticalCenter
                anchors.right : parent.right
                anchors.rightMargin: 6
                spacing: 4
                opacity : 0.26
                Controls.FlatButton{
                    icon : "../images/newcatalog_left_white.png"
                    label : qsTr("Left")
                    tooltip: qsTr("Creates a new catalog in the left panel with the content\n of the last active catalog")
                    onClicked: {
                        bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "left")
                    }
                }
                Controls.FlatButton{
                    icon : "../images/newcatalog_right_white.png"
                    label : qsTr("Right")
                    tooltip: qsTr("Creates a new catalog in the right panel with the content\n of the last active catalog")
                    onClicked: {
                        onClicked: {
                            bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "right")
                        }
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("Temporary")
                    tooltip : qsTr("Catalog that holds objects that are only available this session.\nThey will not be available in when ilwis is restarted")
                    onClicked: {
                        var url = "ilwis://internalcatalog"
                        mastercatalog.selectedBookmark(url)
                        var filter = mastercatalog.filter(url)
                        bigthing.changeCatalog(filter,"catalog", url)
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("System")
                    tooltip : qsTr("Catalog that shows all system objects in ilwis\n System objects are read-only objects that are always available")
                    onClicked: {
                        var url = "ilwis://system"
                        mastercatalog.selectedBookmark(url)
                        var filter = mastercatalog.filter(url)
                        bigthing.changeCatalog(filter,"catalog", url)
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("Operations")
                    tooltip : qsTr("Catalog that shows all available operations and workflows in ilwis")
                    onClicked: {
                        var url = "ilwis://operations"
                        mastercatalog.selectedBookmark(url)
                        var filter = mastercatalog.filter(url)
                        bigthing.changeCatalog(filter,"catalog", url)
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("Bookmark 1")
                    tooltip : mastercatalog.getUrl("bookmark 3")
                    onClicked: {
                        var url = "bookmark 3"
                        var filter = mastercatalog.filter(url)
                        if ( filter !== ""){
                            url = mastercatalog.getUrl(url)
                            mastercatalog.selectedBookmark(url)
                            bigthing.changeCatalog(filter,"catalog", url)
                        }
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("Bookmark 2")
                    tooltip : mastercatalog.getUrl("bookmark 4")
                    onClicked: {
                        var url = "bookmark 4"
                        var filter = mastercatalog.filter(url)
                        if ( filter !== ""){
                            url = mastercatalog.getUrl(url)
                            mastercatalog.selectedBookmark(url)
                            bigthing.changeCatalog(filter,"catalog", url)
                        }
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("Bookmark 3")
                    tooltip : mastercatalog.getUrl("bookmark 5")
                    onClicked: {
                        var url = "bookmark 5"
                        var filter = mastercatalog.filter(url)
                        if ( filter !== ""){
                            url = mastercatalog.getUrl(url)
                            mastercatalog.selectedBookmark(url)
                            bigthing.changeCatalog(filter,"catalog", url)
                        }
                    }
                }
            }
            MouseArea {
                width : childrenRect.width + 2
                height : parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.right : parent.right
                anchors.rightMargin: 6
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    buttonRow.opacity = 1
                }
                onExited: {
                   if ( mouseY >= height || mouseY <= 1)
                        buttonRow.opacity = 0.26
                }
                z : -1
            }

        }

    }
    DropShadow {
        id: cmlShadow
        z: topRoot.z + 1
        anchors.fill: source
        cached: true
        verticalOffset: 4
        radius: 4
        samples: 12
        color: "#80000000"
        smooth: true
        source: topItem
    }

}
