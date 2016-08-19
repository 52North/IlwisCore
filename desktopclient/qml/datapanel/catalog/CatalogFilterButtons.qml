import QtQuick 2.0

Row {
    width : 295
    height : 22
    spacing: 1

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


    ToolBarButton{
        id :showAll
        iconSource: iconsource("all20.png")
        onClicked: toggleFilter("all", checked);
        tooltipText: qsTr("Show or Hide all  objects in the current catalog")
    }

    ToolBarButton{
        id :showRasters
        iconSource: iconsource("raster20CS1.png")
        onClicked: toggleFilter("rastercoverage", checked);
        tooltipText: qsTr("Show or Hide all  raster coverages in the current catalog")
    }
    ToolBarButton{
        id :showFeatures
        iconSource: iconsource("feature20CS1.png")
        onClicked: toggleFilter("featurecoverage", checked);
       tooltipText: qsTr("Show or Hide all  feature coverages in the current catalog")
    }
    ToolBarButton{
        id :showTables
        iconSource: iconsource("table20CS1.png")
        onClicked: toggleFilter("table", checked);
       tooltipText: qsTr("Show or Hide all  tables in the current catalog")
    }
    ToolBarButton{
        id :showCsys
        iconSource: iconsource("csy20.png")
        onClicked: toggleFilter("coordinatesystem", checked);
       tooltipText: qsTr("Show or Hide all  coordinate systems in the current catalog")
    }

    ToolBarButton{
        id :showGrfs
        iconSource: iconsource("georeference20.png")
        onClicked: toggleFilter("georeference", checked);
       tooltipText: qsTr("Show or Hide all  georeferences in the current catalog")
    }
    ToolBarButton{
        id :showDomains
        iconSource: iconsource("domain.png")
        onClicked: toggleFilter("domain", checked);
       tooltipText: qsTr("Show or Hide all  domains in the current catalog")
    }
    ToolBarButton{
        id :showRpr
        iconSource: iconsource("representation20.png")
        onClicked: toggleFilter("representation", checked);
       tooltipText: qsTr("Show or Hide all representations objects in the current catalog")
    }
    ToolBarButton{
        id :showProj
        iconSource: iconsource("projection20.png")
        onClicked: toggleFilter("projection", checked);
       tooltipText: qsTr("Show or Hide all  projections in the current catalog")
    }

    ToolBarButton{
        id :showEll
        iconSource: iconsource("ellipsoid20.png")
        onClicked: toggleFilter("ellipsoid", checked);
       tooltipText: qsTr("Show or Hide all  ellipsoids in the current catalog")
    }
    ToolBarButton{
        id :showOper
        iconSource: iconsource("operation20.png")
        onClicked: toggleFilter("operation", checked);
       tooltipText: qsTr("Show or Hide all  operations in the current catalog")
    }

}

