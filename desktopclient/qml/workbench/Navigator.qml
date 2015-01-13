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
        id : createCatalog
        onTriggered : {
            bigthing.addCatalog()
        }

    }

    Item {
        id : buttonBar
        height : 46
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
                buttontext : qsTr("Open")
                width : 95
                height : 40
                checkable: true
                checked : false
                action : changeCatalogContent
             }
            Controls.ActionButton{
                id : removeBookmark
                width : 95
                height : 40
                iconsource: "../images/deletebookmarkCS1.png"
                buttontext :  "Delete\nBookmark"
                action : deleteBookmark

            }
            Controls.ActionButton{
                id : addCatalog
                width : 95
                height : 40
                iconsource: "../images/newcatalogCS1.png"
                buttontext :  "New\nCatalog"
                action : createCatalog

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
