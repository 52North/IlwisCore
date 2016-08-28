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
        if ( currentCatalog.url.indexOf("ilwis://system") != -1)
            return

        var formatstrings = ""
        var idstring = drag.source.ids
        if ( idstring === "?" || idstring === "")
            idstring = drag.source.ilwisobjectid

        if ( idstring === "")
            return

        var ids = idstring.split("|")
        for(var i=0; i < ids.length; ++i ){

            var obj = mastercatalog.id2object(ids[i], catalogViewsArea)
            console.debug(obj.name)
            if (obj) {
                if ( currentCatalog.url == "ilwis://internalcatalog")
                    formatstrings = "stream:" + obj.typeName;
                else if ( actionBar.useDropFormat())
                    formatstrings = actionBar.getFormatString(drag.source.type)
                if (formatstrings === "")
                    formatstrings = obj.externalFormat
                if ( formatstrings){
                    var parts = formatstrings.split(":")
                    var newUrl = currentCatalog.url + "/" + obj.name
                    obj.copy(newUrl, parts[1].trim(), parts[0].trim());
                }
            }
        }
        currentCatalog.scanContainer(false,true)
    }
    TabView {
        id : catalogPanels
        anchors.fill: parent

        property var tooltips : ["Bulk view in grid layout for available ilwis objects",
            "More detailed view with some metadata in list layout",
            "Preview on coverages in Grid Layout",
            "Spatial location of coverages in the active catalog"];

        
    style: Base.TabStyle1{
        minimumWidth: 60* Global.uiScale
        backgroundColor: catalogSplit.backgroundCatalogColor
    }

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
                    onShowObject : { showObject(objectid)}
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

