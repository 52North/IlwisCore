import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import UIContextModel 1.0
import LayerManager 1.0
import "../../workbench/propertyform" as MetaData
import "../../controls" as Controls
import "../../Global.js" as Global
import "../.." as Base

Column {
    id : column
    property int itemHeight : 16
    property int fSize : 7
    width : parent.width
    height : 120
    anchors.fill: parent

    function setEnvelope(env, ismin){
        if ( !env || !("minx" in env))
            return ""
        if ( ismin)
            return env.minx.toFixed(4) + " " + env.miny.toFixed(4)
        else
            return env.maxx.toFixed(4) + " " + env.maxy.toFixed(4)

    }

    function zoomEnvelope(newenvelope){
        var parts = newenvelope.split(" ")
        zoomEnvelope1.content =  parseFloat(parts[0]).toFixed(4) + " " + parseFloat(parts[1]).toFixed(4)
        zoomEnvelope2.content =  parseFloat(parts[2]).toFixed(4) + " " + parseFloat(parts[3]).toFixed(4)
    }

    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        content : renderer.viewerId

        labelText : qsTr("Viewer id")

    }

    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        content : metatdata.manager.coordinateSystem ? metatdata.manager.coordinateSystem.displayName : ""

        labelText : qsTr("Coordinate System")

    }
    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        content : metatdata.manager.coordinateSystem ? metatdata.manager.coordinateSystem.coordinateSystemName : ""
        labelText : qsTr("Projection")

    }
    Controls.TextEditLabelPair{
        id : viewenv
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        labelText : qsTr("View Envelope")
        content : metatdata.manager.coordinateSystem ? column.setEnvelope(metatdata.manager.viewEnvelope, true) : ""

    }
    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        labelText :""
        fontSize: fSize
        content : metatdata.manager.coordinateSystem ? column.setEnvelope(metatdata.manager.viewEnvelope, false) : ""

    }
    Controls.TextEditLabelPair{
        id : zoomEnvelope1
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        labelText : qsTr("Zoom Envelope ")

    }
    Controls.TextEditLabelPair{
        id : zoomEnvelope2
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        labelText : ""
    }

    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        content : metatdata.manager.coordinateSystem ? column.setEnvelope(metatdata.manager.latlonEnvelope, true) : ""
        labelText : qsTr("Lat/Lon Envelope")

    }
    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        content : metatdata.manager.coordinateSystem ? column.setEnvelope(metatdata.manager.latlonEnvelope, false) : ""
    }
    Controls.TextEditLabelPair{
        height : column.itemHeight
        width : parent.width
        labelWidth: 115
        fontSize: fSize
        content : ""
        labelText : qsTr("Georeference")
    }
}


