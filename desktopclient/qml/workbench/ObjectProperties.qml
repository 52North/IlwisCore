import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import IlwisObjectModel 1.0
import CatalogModel 1.0
import "propertyform" as PropertyForm

WorkBenchShifter {
    id : propertyFormContainer

    function getSelectedData() {
        if ( mastercatalog.hasSelectedObjects()){
           return mastercatalog.selectedData
        }  else if ( mastercatalog.currentCatalog){
            return mastercatalog.currentCatalog.selectedData
        }
    }

    clip : true
    state : "invisible"
    height : parent.height

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/metadata.png"
        headerText:"Object properties"
        z : 1
    }
    ListView {
        id : props
        objectName: "object_properties_list_mainui"
        anchors.top: functionBar.bottom
        width : parent.width
        anchors.bottom: parent.bottom
        model : width > 0 ? mastercatalog.selectedData : null
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
