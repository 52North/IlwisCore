import QtQuick 2.0

Row {
    width : 295
    height : 22
    spacing: 1
    anchors.top: parent.top
    anchors.topMargin: 2

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

