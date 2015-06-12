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

Item {
    Column {
        id : column
        property int itemHeight : 16
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

        Controls.TextEditLabelPair{
            height : column.itemHeight
            width : parent.width
            labelWidth: 115
            content : metatdata.manager.coordinateSystem ? metatdata.manager.coordinateSystem.displayName : ""


            labelText : qsTr("Coordinate System")

        }
        Controls.TextEditLabelPair{
            height : column.itemHeight
            width : parent.width
            labelWidth: 115
            content : metatdata.manager.coordinateSystem ? metatdata.manager.coordinateSystem.coordinateSystemName : ""
            labelText : qsTr("Projection")

        }
        Controls.TextEditLabelPair{
            id : viewenv
            height : column.itemHeight
            width : parent.width
            labelWidth: 115
            labelText : qsTr("View Envelope")
            content : metatdata.manager.coordinateSystem ? column.setEnvelope(metatdata.manager.viewEnvelope, true) : ""

        }
        Controls.TextEditLabelPair{
            height : column.itemHeight
            width : parent.width
            labelWidth: 115
            labelText :""
            content : metatdata.manager.coordinateSystem ? column.setEnvelope(metatdata.manager.viewEnvelope, false) : ""

        }
        Controls.TextEditLabelPair{
            height : column.itemHeight
            width : parent.width
            labelWidth: 115
            content : column.setEnvelope(renderer.zoomEnvelope, true)
            labelText : qsTr("Zoom Envelope ")
        }
        Controls.TextEditLabelPair{
            height : column.itemHeight
            width : parent.width
            labelWidth: 115
            content : column.setEnvelope(renderer.zoomEnvelope, false)
            labelText : ""
        }

        Controls.TextEditLabelPair{
            height : column.itemHeight
            width : parent.width
            labelWidth: 115
            content : ""
            labelText : qsTr("Lat/Lon Envelope")

        }
        Controls.TextEditLabelPair{
            height : column.itemHeight
            width : parent.width
            labelWidth: 115
            content : ""
            labelText : qsTr("Georeference")
        }
    }
}

