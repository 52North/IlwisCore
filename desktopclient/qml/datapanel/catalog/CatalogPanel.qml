import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import CatalogModel 1.0
import MasterCatalogModel 1.0
import CatalogFilterModel 1.0
import TabModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    property int heightButtons :22
    property CatalogModel currentCatalog
    property TabModel tabmodel
    property string panelType : "catalogpanel"
    id : catalogViews
    width : parent.width
    height : parent.height

    onTabmodelChanged: {
        if ( cbuttonBar && tabmodel)
            cbuttonBar.side = tabmodel.side
    }

    signal catalogChanged()

    function showObject(objectid){
        var filter
        if ( objectid === -1){
            var container = currentCatalog.container
            filter = "container='" + container + "'"
            datapanesplit.changePanel(filter, "catalog", container)
        }else {
            var type = mastercatalog.id2type(objectid)
             var newPanel = null
            if ( !type)
                return
            var resource = mastercatalog.id2Resource(objectid)
            if ( resource.typeName === "catalog" || resource.hasExtendedType("catalog")){
                filter = "container='" + resource.url + "'"
                newPanel = datapanesplit.changePanel(filter, "catalog",resource.url)
            }else {
                filter = "itemid=" + resource.id
                newPanel =datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
                if ( newPanel == null){
                    mastercatalog.setSelectedObjects(objectid)
                    bigthing.getWorkbenchPane("objectproperties","visible");
                }
            }
            if ( resource && newPanel){
                resource.makeParent(newPanel) // set the parent correctly as it needs to go as the panels goes and not when the mastercatalog goes(the default parent)
            }
        }
    }

    function addDataSource(filter, sourceName, sourceType){
        var url = sourceName
        if ( currentCatalog)
            currentCatalog.makeParent(0) // begone though evil creature

        currentCatalog = mastercatalog.newCatalog(url,filter)
        if ( currentCatalog){
            currentCatalog.makeParent(catalogViews)
            mastercatalog.currentCatalog = currentCatalog
        }
    }

    function toggleFilter(objecttype, togglestate){
        if ( objecttype === "all"){
            showRasters.checked = showAll.checked;
            showFeatures.checked = showAll.checked;
            showCsys.checked = showAll.checked;
            showGrfs.checked = showAll.checked;
            showTables.checked = showAll.checked;
            showDomains.checked = showAll.checked;
            showRpr.checked = showAll.checked
            showProj.checked = showAll.checked
            showEll.checked = showAll.checked
            showOper.checked = showAll.checked
        }

        currentCatalog.filterChanged(objecttype, togglestate)
        catalogChanged()
    }

    function setResources(){
        if ( currentCatalog)
            return currentCatalog.resources
    }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }


    Rectangle{
        id : toolbar
        anchors.top : parent.top
        width : parent.width
        height : 52
        border.width: 1
        border.color: "grey"
        color : Global.alternatecolor5
        z : 1

        Action {
            id :refreshCatalog
            onTriggered: {
                mastercatalog.refreshWorkingCatalog()
            }
        }


        Row {
            id : objectfilters
            width : 295
            height : 22
            spacing: 1
            anchors.top: parent.top
            anchors.topMargin: 2


            ToolBarButton{
                id : showAll
                iconSource: iconsource("all20.png")
                onClicked: toggleFilter("all", checked);
            }

            ToolBarButton{
                id : showRasters
                iconSource: iconsource("raster20CS1.png")
                onClicked: toggleFilter("rastercoverage", checked);
            }
            ToolBarButton{
                id : showFeatures
                iconSource: iconsource("feature20CS1.png")
                onClicked: toggleFilter("featurecoverage", checked);
            }
            ToolBarButton{
                id : showTables
                iconSource: iconsource("table20CS1.png")
                onClicked: toggleFilter("table", checked);
            }
            ToolBarButton{
                id : showCsys
                iconSource: iconsource("csy20.png")
                onClicked: toggleFilter("coordinatesystem", checked);
            }

            ToolBarButton{
                id : showGrfs
                iconSource: iconsource("georeference20.png")
                onClicked: toggleFilter("georeference", checked);
            }
            ToolBarButton{
                id : showDomains
                iconSource: iconsource("domain.png")
                onClicked: toggleFilter("domain", checked);
            }
            ToolBarButton{
                id : showRpr
                iconSource: iconsource("representation20.png")
                onClicked: toggleFilter("representation", checked);
            }
            ToolBarButton{
                id : showProj
                iconSource: iconsource("projection20.png")
                onClicked: toggleFilter("projection", checked);
            }

            ToolBarButton{
                id : showEll
                iconSource: iconsource("ellipsoid20.png")
                onClicked: toggleFilter("ellipsoid", checked);
            }
            ToolBarButton{
                id : showOper
                iconSource: iconsource("operation20.png")
                onClicked: toggleFilter("operation", checked);
            }

        }
        ComboBox{
            id : catalogfilters
            anchors.top : objectfilters.bottom
            anchors.left : objectfilters.left
            anchors.leftMargin: 2
            anchors.topMargin: 2
            width : 170
            height : 22
            model : mastercatalog.defaultFilters
            textRole: "name"
            onCurrentIndexChanged: {
                if ( tabmodel)
                    tabmodel.selectTab()
                if ( currentIndex > 0){ // first entry is a default empty one
                    var filter = model[currentIndex].catalogQuery
                    var url = model[currentIndex].url
                    if ( url === "")
                        url = "ilwis://mastercatalog"
                    mastercatalog.selectedBookmark(url)
                    bigthing.changeCatalog(filter,"catalog", url)
                }

            }
        }

        Row {
            id : layoutfilter
            width :87
            height : 22
            anchors.top : objectfilters.bottom
            anchors.topMargin: 2
            anchors.left : catalogfilters.right
            anchors.leftMargin: 4

            ExclusiveGroup { id : catalogViewStatus
                onCurrentChanged: {
                    if( showList.checked)
                        catalogView.state = "iconList"
                    if ( showGrid.checked)
                        catalogView.state = "iconGrid"
                    if ( showThumbs.checked)
                        catalogView.state = "thumbList"
                }
            }
            ToolBarButton{
                id : showList
                iconSource: iconsource("listCS1.png")
                exclusiveGroup: catalogViewStatus
                checked : false
            }
            ToolBarButton{
                id : showGrid
                iconSource: iconsource("gridCS1.png")
                checked : true
                exclusiveGroup: catalogViewStatus
            }
            ToolBarButton{
                id : showThumbs
                iconSource: iconsource("thumblistCS1.png")
                exclusiveGroup: catalogViewStatus
                checked: false
            }
        }
        ToolBarButton{
            id : showAnim
            anchors.left : layoutfilter.right
            anchors.top : objectfilters.bottom
            anchors.topMargin: 2
            enabled : currentCatalog ? currentCatalog.canBeAnimated : false
            opacity : currentCatalog ? (currentCatalog.canBeAnimated ? 1 : 0.0) : 0.0
            iconSource: iconsource("animation20.png")
            checkable: false
            checked : false
        }
    }
    Item {
        id : catalogView
        width : parent.width
        anchors.top: toolbar.bottom
        anchors.bottom: cbuttonBar.top
        Connections{
            target : iconListView
            onShowObject : { showObject(objectid,"")}
        }

        CatalogGrid{
            id : iconGridView
            height : parent.height
            opacity : 1
        }
        CatalogTable{
            id : iconListView
            opacity : 0
            height : 0
            enabled : false
        }
        CatalogThumbGrid{
            id : thumbListView
            height : 0
            enabled : false
            opacity: 0

        }
        CatalogMap{
            id : catalogMapView
            height : 0
            enabled : false
            opacity: 0
        }


        states: [
            State {
                name : "iconGrid"
                PropertyChanges { target: thumbListView; height : 0; opacity : 0; enabled : false}
                PropertyChanges { target: iconListView; height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: catalogMapView; height : 0; opacity : 0; enabled : false}
                PropertyChanges { target: iconGridView; height : parent.height; opacity : 1;enabled : true}
            },
            State {
                name : "thumbList"
                PropertyChanges { target: thumbListView; height : parent.height;opacity : 1; enabled : true}
                PropertyChanges { target: iconListView; height :0; opacity : 0;enabled : false}
                PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: catalogMapView; height : 0; opacity : 0; enabled : false}
            },
            State {
                name : "iconList"
                PropertyChanges { target: thumbListView; height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: iconListView; height : parent.height;opacity : 1; enabled : true}
                PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: catalogMapView; height : 0; opacity : 0; enabled : false}
            },
            State {
                name : "bylocation"
                PropertyChanges { target: thumbListView; height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: iconListView; height :0; opacity : 0;enabled : false}
                PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: catalogMapView; height : parent.height;opacity : 1; enabled : true}
            }

        ]

        transitions: [
            Transition {
                ParallelAnimation{
                    NumberAnimation { target: thumbListView; properties: "height"; duration: 400; easing.type: Easing.InOutQuad }
                    NumberAnimation { target: thumbListView; properties: "opacity"; duration: 400; easing.type: Easing.InOutQuad }
                }
                ParallelAnimation{
                    NumberAnimation { target: iconListView; property: "height"; duration: 400; easing.type: Easing.InOutQuad }
                    NumberAnimation { target: iconListView; property: "opacity"; duration: 400; easing.type: Easing.InOutQuad }
                }
                ParallelAnimation{
                    NumberAnimation { target: iconGridView; property: "height"; duration: 400; easing.type: Easing.InOutQuad }
                    NumberAnimation { target: iconGridView; property: "opacity"; duration: 400; easing.type: Easing.InOutQuad }
                }
            }
        ]
    }
    CatalogButtonBar{
        id : cbuttonBar
        anchors.bottom: parent.bottom
        width : parent.width
    }
    Component.onCompleted: {
    }

}
