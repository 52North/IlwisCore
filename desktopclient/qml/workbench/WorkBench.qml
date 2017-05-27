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
    property var datapanel : null

     function transitionInfoPane(newpage,visstate) {
         var oldpage = currentpage
         if ( newpage === "navigator")
             currentpage = navigator
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
         if ( newpage === "preferences")
             currentpage = userpreferences
         if ( oldpage && (currentpage === oldpage) ){
             if ( visstate === "toggle")
                oldpage.state = oldpage.state === "invisible" ? "visible" : "invisible"
             else if ( visstate === "visible"){
                currentpage.state = "visible"
             } else
                currentpage.state = "invisible"
         }
         if ( currentpage !== oldpage){
             currentpage.state = (visstate === "toggle" || visstate === "visible")? "visible" : "invisible"
             if ( oldpage)
                oldpage.state = "invisible"
         }
        if ( currentpage){
            workBench.state = currentpage.state
        }
        else
            workBench.state = "visible"
     }


    function currentPane() {
        var currentloader = infoPContent1.source.toString().indexOf(lastPage) !== -1 ? infoPContent1 : infoPContent2
        return currentloader.item;
    }

    width : defaultFunctionBarWidth
    color :  uicontext.workbenchBGColor

    Navigator{ id : navigator}
    Tranquilizers{ id : tranquilizers}
    OperationSelection{ id : operationselection}
    ObjectCreation{ id : objectcreation}
    MessagesPane{ id : messages}
    Info{id : info}
    ObjectProperties{ id : objectproperties}
    Preferences{id : userpreferences}


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
