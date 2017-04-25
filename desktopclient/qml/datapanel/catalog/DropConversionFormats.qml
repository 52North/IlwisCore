import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import PreferencesModel 1.0
import QtQuick.Controls.Styles 1.1
import "../../controls" as Controls
import "../../Global.js" as Global


Column {
    width : 290
    height : childrenRect.height
    spacing : 3

    function getFormatString(requestedType){
        if ( requestedType === "rastercoverage"){
            return rasters.selectedFormatString()
        }
        if ( requestedType === "featurecoverage" ||
                requestedType === "polygoncoverage" ||
                requestedType === "linecoverage" ||
                requestedType === "pointcoverage"){
            return features.selectedFormatString()
        }
        if ( requestedType === "table" || requestedType === "flattable" || requestedType === "attributetable"){
            return table.selectedFormatString()
        }
        if ( requestedType === "coordinatesystem" || requestedType === "conventionalcoordinatesystem"){
            return csy.selectedFormatString()
        }
        if ( requestedType === "domain"){
            return dom.selectedFormatString()
        }
        return ""
    }

    function providersQuery(){
        var query = ""
        if ( currentCatalog) {
            var prov = currentCatalog.dataProviders

            for(var i=0; i < prov.length; ++i){
                if ( query != "")
                    query += " or "
                query += "connector=\'" + prov[i] +"\'"
            }
            query =  " and (" + query +")"
        }
        return "(datatype & %1)!=0 and (readwrite='rc' or readwrite='rcu')" + query
    }

    Controls.FormatsComboBox{
        id : rasters
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("rastercoverage");
        labelText: qsTr("Raster coverage")
        labelWidth: 120
        query : providersQuery()

        Controls.ToolTip{
            target: rasters
            text : qsTr("Raster coverages dropped in this catalog will be converted to the indicated format")
        }

        Component.onCompleted: {
            var name = preferences.preferedDataFormat("rastercoverage")
            if ( name !== "?")
                rasters.select(name)
            else
                rasters.select("Ilwis 4 rastercoverage")


        }
        Connections {
            target: rasters
            onIndexChanged :{
                preferences.setPreferedDataFormat("rastercoverage",rasters.comboText)
            }
        }

    }

    Controls.FormatsComboBox{
        id : features
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("featurecoverage");
        labelText: qsTr("Feature coverage")
        labelWidth: 120
        query : providersQuery()

        Controls.ToolTip{
            target: features
            text : qsTr("Feature coverages dropped in this catalog will be converted to the indicated format")
        }

        Component.onCompleted: {
            var name = preferences.preferedDataFormat("featurecoverage")
            if ( name !== "?")
                features.select(name)
            else
                features.select("Ilwis 4 featurecoverage")

        }
        Connections {
            target: rasters
            onIndexChanged :{
                preferences.setPreferedDataFormat("featurecoverage",features.comboText)
            }
        }

    }

    Controls.FormatsComboBox{
        id : table
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("table");
        labelText: qsTr("Table")
        labelWidth: 120
         query : providersQuery()

        Controls.ToolTip{
            target: table
            text : qsTr("Tables dropped in this catalog will be converted to the indicated format")
        }

        Component.onCompleted: {
            var name = preferences.preferedDataFormat("table")
            if ( name !== "?")
                table.select(name)
            else
                table.select("Ilwis 4 table")

        }
        Connections {
            target: rasters
            onIndexChanged :{
                preferences.setPreferedDataFormat("table",table.comboText)
            }
        }

    }
    Controls.FormatsComboBox{
        id : csy
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("coordinatesystem");
        labelText: qsTr("CoordinateSystem")
        labelWidth: 120
        query : providersQuery()

        Controls.ToolTip{
            target: csy
            text : qsTr("Coordinate system dropped in this catalog will be converted to the indicated format")
        }

        Component.onCompleted: {
            var name = preferences.preferedDataFormat("coordinatesystem")
            if ( name !== "?")
                csy.select(name)
            else
                csy.select("Ilwis 4 coordinatesystem")

        }
        Connections {
            target: rasters
            onIndexChanged :{
                preferences.setPreferedDataFormat("coordinatesystem",csy.comboText)
            }
        }

    }

    Controls.FormatsComboBox{
        id : dom
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("domain");
        labelText: qsTr("Domain")
        labelWidth: 120
        query : providersQuery()

        Controls.ToolTip{
            target: dom
            text : qsTr("Domains dropped in this catalog will be converted to the indicated format")
        }

        Component.onCompleted: {
            var name = preferences.preferedDataFormat("domain")
            if ( name !== "?")
                dom.select(name)
            else
                dom.select("Ilwis 4 domain")

        }
        Connections {
            target: rasters
            onIndexChanged :{
                preferences.setPreferedDataFormat("domain",dom.comboText)
            }
        }
    }
    Controls.FormatsComboBox{
        id : catalog
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("catalog");
        labelText: qsTr("Catalog")
        labelWidth: 120
        enabled : false // future
        query : providersQuery()

        Controls.ToolTip{
            target: dom
            text : qsTr("Catalogs dropped in this catalog will be converted to the indicated format")
        }

        Component.onCompleted: {
            var name = preferences.preferedDataFormat("catalog")
            if ( name !== "?")
                catalog.select(name)
            else
                catalog.select("Ilwis 4 catalog")

        }
        Connections {
            target: rasters
            onIndexChanged :{
                preferences.setPreferedDataFormat("domain",dom.comboText)
            }
        }
    }

}
