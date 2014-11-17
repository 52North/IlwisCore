import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MessageModel 1.0
import ResourceModel 1.0

Rectangle {
    id : workbench

    property string lastPage
    property int rightMargin : 18 // creates space for potential scrollbars

     function transitionInfoPane(newpagename) {
          if ( newpagename !== lastPage) {
            if ( lastPage == "")
                workbench.state = "visible"
            else if ( workbench.state == "invisible")
                workbench.state = "visible"

            var currentloader = infoPContent1.source.toString().indexOf(lastPage) !== -1 ? infoPContent1 : infoPContent2;
            var newloader = infoPContent2.source.toString().indexOf(lastPage) !== -1 ? infoPContent2 : infoPContent1;

            currentloader.item.state = "invisible"

            newloader.source = newpagename
            newloader.item.state = "visible"
            newloader.enabled = true
            lastPage = newpagename;
            currentloader.z = 0
            newloader.z = 1
        }else {
            workbench.state = workbench.state === "visible" ? "invisible" : "visible"
        }

    }

    function unloadcontent(pagename) {
        if (infoPContent1.source.toString().indexOf(pagename) != -1) {
            infoPContent1.enabled = false
        } else  if (infoPContent2.source.toString().indexOf(pagename) != -1) {
            infoPContent2.enabled = false
        }
    }

    width : defaultFunctionBarWidth
    color : background4
    Loader {
        id : infoPContent1
        y : 5
        x : 5
        width: workbench.width - rightMargin
        height : parent.height - 15
    }

    Loader {
        id : infoPContent2
        y : 5
        x : 5
        width: workbench.width - rightMargin
        height : parent.height - 15

    }

    Connections {
         target: infoPContent1.item
         onUnloadcontent: unloadcontent(content)
     }

    Connections {
         target: infoPContent2.item
         onUnloadcontent: unloadcontent(content)
     }

    states: [
        State { name: "visible"

            PropertyChanges {
                target: workbench
                width : defaultFunctionBarWidth
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: workbench
                width : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "width"; duration : 750 ; easing.type: Easing.InOutCubic }
        }
    ]
    Component.onCompleted: {
        infoPContent1.source = "Navigator.qml"
    }


}
