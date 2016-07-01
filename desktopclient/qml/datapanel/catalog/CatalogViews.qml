import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import CatalogModel 1.0
import IlwisObjectModel 1.0
import "../.." as Base
import "../../Global.js" as Global

DropArea {
    id : catalogViewsArea
    width : parent.width
    state : "maxed"

    onDropped: {
        var formatstrings
        var obj = mastercatalog.id2object(drag.source.ilwisobjectid, catalogViewsArea)
        console.debug(actionBar.useDropFormat())
        if ( actionBar.useDropFormat())
            formatstrings = actionBar.getFormatString(drag.source.type)
        if (!formatstrings)
            formatstrings = obj.externalFormat.split(":")
        var newUrl = currentCatalog.url + "/" + obj.name
        //obj.copy(newUrl, formatstrings[1].trim(), formatstrings[0].trim());
        currentCatalog.refresh()
    }
    TabView {
        id : catalogPanels
        anchors.fill: parent

        
    style: Base.TabStyle1{ minimumWidth: 60* Global.uiScale}

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

    }
    states: [
    State {
             name : "sized"
             PropertyChanges { target: catalogViewsArea; height : parent.height - Global.actionBarMaxHeight}
         },
         State {
             name : "maxed"
             PropertyChanges { target: catalogViewsArea; height : parent.height - Global.actionBarMinHeight}
         }
    ]
    transitions: [
        Transition {
            ParallelAnimation {
                NumberAnimation { target : catalogViewsArea; properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
            }
        }
    ]
}

