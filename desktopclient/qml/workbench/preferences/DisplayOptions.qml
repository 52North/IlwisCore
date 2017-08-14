import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import PreferencesModel 1.0

Column {
    id : displayOptionsPane
    width: parent.width
    height: 0
    opacity : 0
    state : "minimized"

    Row {
        height : displayOptions.state == "minimized" ? 0 : 25
        enabled: displayOptions.state != "minimized"
        width : parent.width - 10
        y : 10
        x : 10
        spacing : 5
        Controls.TextEditLabelPair{
            id : scaleValue
            width : 200
            height : Global.rowHeight
            content : preferences.uiScale
            labelText: qsTr("User Interface Scale")
            labelWidth: 140
        }
        Button {
            width : 40
            height : 20
            text : qsTr("Apply")
            onClicked:{
                preferences.uiScale = parseFloat(scaleValue.content)
            }
        }


    }
    Row {
        id : colorpickers
        width : parent.width
        height : displayOptions.state == "minimized" ? 0 : 25
        enabled: displayOptions.state != "minimized"
        ComboBox {
            id : colors
            width : 140
            height : 20
            model : ["Selected Headers", "Workbench buttons" , "Selected buttons","Top Header", "App Backgrounds" , "Color 6","Form background","Workbench Background","Catalog background","Generic background",
                "Action Buttons","Action Header" ]
        }


        function changeColor(index, color){
            switch (index){
            case 0:
                if ( uicontext.paleColor !== color){
                    uicontext.paleColor = color
                }
                break;
            case 1:
                if ( uicontext.middleColor !== color){
                    uicontext.middleColor = color
                }
                break;
            case 2:
                if ( uicontext.darkColor !== color){
                    uicontext.darkColor = color
                }
                break;
            case 3:
                if ( uicontext.darkestColor !== color){
                    uicontext.darkestColor = color
                }
                break;
            case 4:
                if ( uicontext.lightColor !== color){
                    uicontext.lightColor = color
                }
                break;
            case 5:
                if ( uicontext.lighterColor !== color){
                    uicontext.lighterColor = color
                }
                break;
            case 6:
                if ( uicontext.lightestColor !== color){
                    uicontext.lightestColor = color
                }
                break;
            case 7:
                if ( uicontext.workbenchBGColor !== color){
                    uicontext.workbenchBGColor = color
                }
                break;
            case 8:
                if ( uicontext.catalogBGColor !== color){
                    uicontext.catalogBGColor = color
                }
                break;
            case 9:
                if ( uicontext.genericBGColor !== color){
                    uicontext.genericBGColor = color
                }
                break;
            case 10:
                if ( uicontext.actionColor !== color){
                    uicontext.actionColor = color
                }
                break;
            case 11:
                if ( uicontext.actionHeaderColor !== color){
                    uicontext.actionHeaderColor = color
                }
                break;
            }
        }

        Controls.ColorPicker2{
            onSelectedColorChanged: {
                colorpickers.changeColor(colors.currentIndex, selectedColor)

            }
            Component.onCompleted: {
               selectedColor = uicontext.paleColor
            }
        }
    }

    states: [
        State { name: "maximized"

            PropertyChanges {
                target: displayOptionsPane
                height : 250
                opacity : 1
            }
        },
        State {
            name : "minimized"
            PropertyChanges {
                target: displayOptionsPane
                height : 0
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : 750 ; easing.type: Easing.InOutCubic }
        }
    ]
}
