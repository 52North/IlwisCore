import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import CatalogModel 1.0
import MasterCatalogModel 1.0
import "../../Global.js" as Global

Item {
    property int heightButtons : 22
    property string tabLocation : "left"
    property CatalogModel currentCatalog
    property bool canSeparate : false
    id : catalogViews
    width : parent.width
    height : parent.height


    signal catalogChanged()

    function showObject(objectid, subtype){
        var type = mastercatalog.id2type(objectid)
        if ( !type)
            return
        var qmlUrl;
        if ( type === "rastercoverage" || type === "featurecoverage" || type === "linecoverage" || type === "pointcoverage" || type === "polygoncoverage")
            qmlUrl = "../visualization/Visualize.qml"
        if ( type === "table" || type === "flattable")
            qmlUrl = "../table/TablePane.qml"
        var component = Qt.createComponent(qmlUrl)
        var resource = mastercatalog.id2Resource(objectid)
        if ( resource !== null){
            var name = resource.displayName
            var blocksize = 24 / 2;
            if ( name.length > 15){
                var part1 = name.substr(0,blocksize)
                var part2 = name.substr( name.length - blocksize)
                name = part1 + "..." + part2

            }
            datapanesplit.showObject(tabLocation,name,component,resource)
        }
    }


    function toggleFilter(objecttype, togglestate){
        if ( objecttype === "all"){
            showRasters.checked = showAll.checked;
            showFeatures.checked = showAll.checked;
            showCoverages.checked = showAll.checked;
            showCsys.checked = showAll.checked;
            showGrfs.checked = showAll.checked;
            showTables.checked = showAll.checked;
            showDomains.checked = showAll.checked;
            showRpr.checked = showAll.checked
            showProj.checked = showAll.checked
            showEll.checked = showAll.checked
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
        color : Global.alternatecolor5

        Action {
            id :refreshCatalog
            onTriggered: {
                mastercatalog.refreshWorkingCatalog()
            }
        }

        RowLayout{
            spacing: 8
            anchors.verticalCenter: parent.verticalCenter
            ExclusiveGroup { id : catalogViewStatus
                onCurrentChanged: {
                    if( showList.checked)
                        catalogView.state = "iconList"
                    if ( showGrid.checked)
                        catalogView.state = "iconGrid"
                    if ( showThumbs.checked)
                        catalogView.state = "thumbList"
                    if ( showByLoc.checked)
                        catalogView.state = "bylocation"
                }
            }
            Grid {
                width : 180
                height : 46
                columns : 6
                spacing: 2


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
                    id : showCoverages
                    iconSource: iconsource("coverage20.png")
                    onClicked: toggleFilter("coverage", checked);
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

            }


            Grid {
                width : 46
                height : 46
                columns : 2
                rows : 2
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
                ToolBarButton{
                    id : showByLoc
                    iconSource: iconsource("location20.png")
                    exclusiveGroup: catalogViewStatus
                    checked : false
                }
            }
            ToolBarButton{
                id : refresh
                implicitHeight: heightButtons
                iconSource: iconsource("refreshCS1.png")
                action : refreshCatalog
            }


        }


    }
    Item {
        id : catalogView
        width : parent.width
        anchors.top: toolbar.bottom
        anchors.bottom: parent.bottom
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
    Component.onCompleted: {
        var url = mastercatalog.currentUrl
        currentCatalog = mastercatalog.newCatalog(url)
        currentCatalog.makeParent(catalogViews)
        mastercatalog.currentCatalog = currentCatalog
    }

}
