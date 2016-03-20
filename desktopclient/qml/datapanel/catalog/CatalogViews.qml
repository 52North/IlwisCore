import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import CatalogModel 1.0
import "../.." as Base

TabView {
    id : catalogPanels
    width : parent.width

    style: Base.TabStyle1{ minimumWidth: 60}

    Tab{
        title : qsTr("Grid")
        CatalogGrid{
            id : iconGridView
            height : parent.height
            opacity : 1
        }
    }
    Tab {
        title : qsTr("List")
        CatalogTable{
            Connections{
                target : iconListView
                onShowObject : { showObject(objectid,"")}
            }
            id : iconListView
            height : parent.height
        }
    }
    Tab {
        title : qsTr("Thumb")
        CatalogThumbGrid{
            id : thumbListView
            height : parent.height

        }
    }
    Tab {
        title : qsTr("Spatial")
        CatalogMap{
            id : catalogMapView
            height : parent.height
        }
    }
    states: [
    State {
             name : "sized"
             PropertyChanges { target: catalogPanels; height : parent.height - 330}
         },
         State {
             name : "maxed"
             PropertyChanges { target: catalogPanels; height : parent.height - 25}
         }
    ]
    transitions: [
         Transition {
             ParallelAnimation {
                 NumberAnimation { target : catalogPanels; properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
              }
         }
     ]
}

