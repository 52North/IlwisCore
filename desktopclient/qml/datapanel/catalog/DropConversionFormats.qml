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
        if ( requestedType === "rastercoverage"){
            return features.selectedFormatString()
        }
        if ( requestedType === "table"){
            return table.selectedFormatString()
        }
        if ( requestedType === "coordinatesystem"){
            return csy.selectedFormatString()
        }
        if ( requestedType === "domain"){
            return dom.selectedFormatString()
        }
        return ""
    }

    Controls.FormatsComboBox{
        id : rasters
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("rastercoverage");
        labelText: qsTr("Raster coverage")
        labelWidth: 120

        Component.onCompleted: {
            var name = preferences.preferedDataFormat("rastercoverage")
            if ( name !== "?")
                rasters.select(name)

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

    }

    Controls.FormatsComboBox{
        id : table
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("table");
        labelText: qsTr("Table")
        labelWidth: 120

    }
    Controls.FormatsComboBox{
        id : csy
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("coordinatesystem");
        labelText: qsTr("CoordinateSystem")
        labelWidth: 120

    }

    Controls.FormatsComboBox{
        id : dom
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("domain");
        labelText: qsTr("Domain")
        labelWidth: 120

    }

}
