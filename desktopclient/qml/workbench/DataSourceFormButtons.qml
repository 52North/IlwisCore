import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0

Item {
    signal bookmarkadded()

    width : parent.width
    height : 24

    Action {
        id :addbookmark
        onTriggered: {
            mastercatalog.addBookmark(currentFolder)
            bookmarkadded()
        }
    }
    Row {
        id : buttonRow
        height :24
        anchors.right : parent.right
        anchors.rightMargin: 35
        anchors.bottomMargin: 5
        width : 45
        Button{
            text : "Add Bookmark"
            id : addbutton
            height : 24
            width :80
            action : addbookmark
            z : 1
        }
    }
    Component.onCompleted: {
    }
}
