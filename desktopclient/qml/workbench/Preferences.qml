import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global

WorkBenchShifter {
    id : messageContainer

    clip : true
    state : "invisible"
    height : parent.height

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/preferencesCS1.png"
        headerText:"Settings"
    }
    Column {
        anchors.top : functionBar.bottom
        anchors.topMargin: 4
        height : parent.height - functionBar.height - 4
        width : 30
        Button{
            id : folders
            width : 50
            height : 50
            Column{
                x : 3
                y : 2
                spacing : 3
                width : parent.width - 3
                height : parent.height - 2
                Image {
                    x : 7
                    width : 25
                    height : 25
                    source: "../images/folderdisk.png"
                    fillMode: Image.PreserveAspectFit
                }
                Text {
                    height : 20
                    width : parent.width
                    text : "locations"
                }
            }
        }
    }
}

