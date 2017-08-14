import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import CatalogModel 1.0
import IlwisObjectModel 1.0
import "../.." as Base
import "../../Global.js" as Global
import "../../controls" as Controls

DropArea {
    id : catalogViewsArea
    width : parent.width
    state : ""


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
        if ( ids.length === 1){ // you can start a drag of a single item without actually selecting it; this means that the selected ids are not necessarily correct. The ilwisobjectid is though
            ids[0] = drag.source.ilwisobjectid
        }

        for(var i=0; i < ids.length; ++i ){
            var obj = mastercatalog.id2object(ids[i], catalogViewsArea)
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

        
    style: Base.TabStyleCatalog{
        minimumWidth: 60
        backgroundColor: uicontext.genericBGColor
    }

        Controls.CatalogTab{
            title : qsTr("Grid")
            iconName: "images/tab_grid"
            CatalogGrid{
                id : iconGridView
                height : parent.height
                opacity : 1
            }
        }
        Controls.CatalogTab {
            title : qsTr("List")
            iconName: "images/tab_list"
            CatalogTable{
                Connections{
                    target : iconListView
                    onShowObject : { showObject(objectid)}
                }
                id : iconListView
                height : parent.height
            }
        }
        Controls.CatalogTab {
            title : qsTr("Thumb")
            iconName: "images/tab_thumb"
            CatalogThumbGrid{
                id : thumbListView
                height : parent.height

            }
        }
        Controls.CatalogTab {
            title : qsTr("Spatial")
            iconName: "images/tab_spatial"
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

