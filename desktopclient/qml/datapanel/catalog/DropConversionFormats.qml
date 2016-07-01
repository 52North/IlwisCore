import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../controls" as Controls
import "../../Global.js" as Global


Column {
    width : 290
    height : childrenRect.height
    spacing : 3

    function getFormatString(requestedType){
        if ( requestedType === "rastercoverage"){
            console.debug(rasters.comboText)
        }
    }

    Controls.FormatsComboBox{
        id : rasters
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("rastercoverage");
        labelText: qsTr("Raster coverage")
        labelWidth: 120

    }

    Controls.FormatsComboBox{
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("featurecoverage");
        labelText: qsTr("Feature coverage")
        labelWidth: 120

    }

    Controls.FormatsComboBox{
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("table");
        labelText: qsTr("Table")
        labelWidth: 120

    }
    Controls.FormatsComboBox{
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("coordinatesystem");
        labelText: qsTr("CoordinateSystem")
        labelWidth: 120

    }

    Controls.FormatsComboBox{
        width : parent.width
        height : 20
        ilwisType: uicontext.typeName2typeId("domain");
        labelText: qsTr("Domain")
        labelWidth: 120

    }

}
