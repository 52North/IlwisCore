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
            color : Global.darkestgreen
            Row {
                width : parent.width - x
                height : parent.height - 2
                spacing: 4
                anchors.verticalCenter: parent.verticalCenter
                x : 5
                Image {
                    width : 54
                    anchors.verticalCenter: parent.verticalCenter
                    source : "images/ilwis_logo_txt.png"
                }
                Controls.FlatButton{
                    icon : "../images/newcatalog_left.png"
                    label : qsTr("Left")
                    onClicked: {
                        bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "left")
                    }
                }
                Controls.FlatButton{
                    icon : "../images/newcatalog_right.png"
                    label : qsTr("Right")
                    onClicked: {
                        onClicked: {
                            bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "right")
                        }
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("Operations")
                    onClicked: {
                        var url = "ilwis://operations"
                        mastercatalog.selectedBookmark(url)
                        var filter = mastercatalog.filter(url)
                        bigthing.changeCatalog(filter,"catalog", url)
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("System")
                    onClicked: {
                        var url = "ilwis://system"
                        mastercatalog.selectedBookmark(url)
                        var filter = mastercatalog.filter(url)
                        bigthing.changeCatalog(filter,"catalog", url)
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("Temporary")
                    onClicked: {
                        var url = "ilwis://internalcatalog"
                        mastercatalog.selectedBookmark(url)
                        var filter = mastercatalog.filter(url)
                        bigthing.changeCatalog(filter,"catalog", url)
                    }
                }
                Controls.FlatButton{
                    icon : "../images/bookmark.png"
                    label : qsTr("Bookmark 1")
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
