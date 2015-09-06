import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../controls" as Controls
import "../Global.js" as Global

Item {
    id : container

    property string startfolder

    signal unloadcontent(string content)
    signal catalogChanged()

    clip : true
    opacity : 0

    function refreshBookmarks() {
        navCreatePanel.bookmarkModel = mastercatalog.bookmarked
    }

    Connections {
         target: navCreatePanel
         onCatalogChanged: { catalogChanged()}
     }

    Connections {
         target: createCatalogForm
         onBookmarkadded: { refreshBookmarks()}
     }

    Connections {
         target: navCreatePanel
         onBookmarksChanged: { refreshBookmarks()}
     }



    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/navigatorCS1.png"
        headerText:qsTr("Navigator")
    }

    Action {
        id : changeCatalogContent
        onTriggered : {
            addContainer.checked = !addContainer.checked
            createCatalogForm.state = addContainer.checked ? "maximized" : "minimized"
            createCatalogForm.opacity = 1
        }
    }

    Action {
        id : deleteBookmark
        onTriggered : {
            navCreatePanel.deleteCurrentBookmark()
        }
    }
    
    Action {
        id : createCatalogLeft
        onTriggered : {
            bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "left")
        }

    }

    Action {
        id : createCatalogRight
        onTriggered : {
            bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "right")
        }

    }

    Item {
        id : buttonBar
        height : 65
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : functionBarHeader.x

        Row {
            anchors.top : parent.top
            anchors.margins: 3
            Controls.ActionButton{
                id : addContainer
                iconsource : "../images/openCS1.png"
                buttontext : qsTr("Add Bookmark")
                width : 82
                height : buttonBar.height - 3
                checkable: true
                checked : false
                action : changeCatalogContent
             }
            Controls.ActionButton{
                id : removeBookmark
                width : 82
                height : buttonBar.height - 3
                iconsource: "../images/deletebookmarkCS1.png"
                buttontext :  "Drop Bookmark"
                action : deleteBookmark

            }
            Controls.ActionButton{
                id : addCatalog
                width : 82
                height : buttonBar.height - 3
                iconsource: "../images/newcatalogCS1.png"
                buttontext :  "New Catalog \n     Left"
                action : createCatalogLeft

            }
            Controls.ActionButton{
                id : addCatalog2
                width : 82
                height : buttonBar.height - 3
                iconsource: "../images/newcatalogCS1.png"
                buttontext :  "New Catalog \n     Right"
                action : createCatalogRight

            }
       }
    }
    CreateCatalogForm{
        id : createCatalogForm
        width : parent.width - 10
        height : 0
        anchors.top: buttonBar.bottom
        anchors.margins: 5
        x : parent.x + 5
        beginfolder: startfolder
    }

    Bookmarks{
        id : navCreatePanel
        width : parent.width - 10
        x : parent.x + 5
        defaultHeight: parent.height - buttonBar.height - functionBarHeader.height - createCatalogForm.height - 15
        anchors.top: createCatalogForm.bottom
        anchors.margins: 2
        opacity : 1
        background: Global.mainbackgroundcolor
        listbackground: "#FFFEF8"
        listalternate: "#FFFEF8"

    }

    states: [
        State { name: "visible"

            PropertyChanges {
                target: container
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: container
                opacity : 0
            }
        }

    ]
      transitions: [
          Transition {
              NumberAnimation {
                  properties: "opacity"; duration : 500 ; easing.type: Easing.Linear
              }
              onRunningChanged :
              {
                  if ( opacity == 0) {
                    unloadcontent("Navigator.qml")
                  }
              }

          }
      ]

      Component.onCompleted: {
          state : "visible"
          opacity : 1
      }

}
