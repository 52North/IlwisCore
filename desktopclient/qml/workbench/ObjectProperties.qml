import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import IlwisObjectModel 1.0
import CatalogModel 1.0
import "propertyform" as PropertyForm

Rectangle {
    id : propertyFormContainer

    signal unloadcontent(string content)

    function getSelectedData() {
        if ( mastercatalog.currentCatalog){
            return mastercatalog.currentCatalog.selectedData
        }
    }

    color : "white"
    clip : true
    state : "invisible"
    height : 500
    opacity : 1

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/propertiesCS1.png"
        headerText:"Object properties"
        z : 1
    }
    ListView {
        id : props
        anchors.top: functionBar.bottom
        width : parent.width
        anchors.bottom: parent.bottom
        model : getSelectedData()
        property int lastIndex : 0
        delegate : PropertyForm.DPropertyForm{}
    }




    states: [
        State { name: "visible"

            PropertyChanges {
                target: propertyFormContainer
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: propertyFormContainer
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
                    unloadcontent("ObjectProperties.qml")
                }
            }

        }
    ]

    Component.onCompleted: {
    }
}
