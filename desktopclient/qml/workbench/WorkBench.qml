import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MessageModel 1.0
import ResourceModel 1.0
import "../Global.js" as Global

Rectangle {
    id : workbench

    property string lastPage
    property int rightMargin : 18 // creates space for potential scrollbars
    property var currentpage : null

     function transitionInfoPane(newpage) {
         var oldpage = currentpage
         if ( newpage === "navigator")
             currentpage = navigator
         if ( newpage === "workspaces")
             currentpage = workspaces
         if ( newpage === "tranquilizers")
             currentpage = tranquilizers
         if ( newpage === "operationselection")
             currentpage = operationselection
         if ( newpage === "objectcreation")
             currentpage = objectcreation
         if ( newpage === "messages")
             currentpage = messages
         if ( newpage === "info")
             currentpage = info
         if ( newpage === "objectproperties")
             currentpage = objectproperties
         if ( currentpage === oldpage ){
             oldpage.state = oldpage.state === "invisible" ? "visible" : "invisible"
         }
         if ( currentpage !== oldpage){
             currentpage.state = "visible"
             if ( oldpage)
                oldpage.state = "invisible"
         }
         workBench.state = currentpage.state
     }


    function currentPane() {
        var currentloader = infoPContent1.source.toString().indexOf(lastPage) !== -1 ? infoPContent1 : infoPContent2
        return currentloader.item;
    }

    width : defaultFunctionBarWidth
    color :  Global.alternatecolor2

    Navigator{ id : navigator}
    Workspaces{ id : workspaces }
    Tranquilizers{ id : tranquilizers}
    OperationSelection{ id : operationselection}
    ObjectCreation{ id : objectcreation}
    MessagesPane{ id : messages}
    Info{id : info}
    ObjectProperties{ id : objectproperties}


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
            NumberAnimation { properties: "width"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]
}
