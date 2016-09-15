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
        iconSource: iconsource("raster.png")
        onClicked: toggleFilter("rastercoverage", checked);
        tooltipText: qsTr("Show or Hide all  raster coverages in the current catalog")
    }
    ToolBarButton{
        id :showFeatures
        iconSource: iconsource("vector.png")
        onClicked: toggleFilter("featurecoverage", checked);
       tooltipText: qsTr("Show or Hide all  feature coverages in the current catalog")
    }
    ToolBarButton{
        id :showTables
        iconSource: iconsource("table.png")
        onClicked: toggleFilter("table", checked);
       tooltipText: qsTr("Show or Hide all  tables in the current catalog")
    }
    ToolBarButton{
        id :showCsys
        iconSource: iconsource("coordinate.png")
        onClicked: toggleFilter("coordinatesystem", checked);
       tooltipText: qsTr("Show or Hide all  coordinate systems in the current catalog")
    }

    ToolBarButton{
        id :showGrfs
        iconSource: iconsource("georeference.png")
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
        iconSource: iconsource("representation.png")
        onClicked: toggleFilter("representation", checked);
       tooltipText: qsTr("Show or Hide all representations objects in the current catalog")
    }
    ToolBarButton{
        id :showProj
        iconSource: iconsource("projection.png")
        onClicked: toggleFilter("projection", checked);
       tooltipText: qsTr("Show or Hide all  projections in the current catalog")
    }

    ToolBarButton{
        id :showEll
        iconSource: iconsource("ellipsoid.png")
        onClicked: toggleFilter("ellipsoid", checked);
       tooltipText: qsTr("Show or Hide all  ellipsoids in the current catalog")
    }
    ToolBarButton{
        id :showOper
        iconSource: iconsource("operation.png")
        onClicked: toggleFilter("operation", checked);
       tooltipText: qsTr("Show or Hide all  operations in the current catalog")
    }

}

