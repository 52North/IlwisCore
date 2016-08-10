import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import "../controls" as Controls
import "../Global.js" as Global

Item {
    id : csyBoundingBox
    property string minx : ""
    property string miny : ""
    property string maxx : ""
    property string maxy : ""

    property string boundingbox : setbb()
    property bool islatlon : false
    property bool readOnly : false

    function setEnvelope(bb){
         valuepairs.item.setBounds(bb)
    }

    function getEnvelope() {
        if ( minx == "" || maxx == "" || maxy == "" || miny == "")
            return ""

        return "("+ minx + " " + miny + "," + maxx + " " + maxy + ")"
    }

    function setbb() {
           if ( valuepairs.item !== undefined && valuepairs.item.metricbb !== undefined){
                return  valuepairs.item.metricbb
           }
           return ""
    }

    Component{
        id :latlon
        Column {
            function setBounds(bb){
                var parts = bb.split(" ")
                if ( parts.length === 4){
                    lonmin.setCoordinate(parts[0])
                    latmin.setCoordinate(parts[1])
                    lonmax.setCoordinate(parts[2])
                    latmax.setCoordinate(parts[3])
                }
                if ( parts.length === 6){
                    lonmin.setCoordinate(parts[0])
                    latmin.setCoordinate(parts[1])
                    lonmax.setCoordinate(parts[3])
                    latmax.setCoordinate(parts[4])
                }
            }

            anchors.fill: parent
            property string metricbb : lonmin.metric + " " + latmin.metric + " " + lonmax.metric + " " + latmin.metric
            Row {
                height : readOnly ? Global.rowHeight - 3 : Global.rowHeight
                width : parent.width
                Text{
                    width : 140
                    height : parent.height
                    text : qsTr("Minimum Latitude")
                    font.bold: true
                }

                Controls.LatLonCoordinate{
                    id : latmin
                    width : 200
                    isReadOnly: readOnly
                    onMetricChanged: {
                        miny = metric
                    }
                }
            }
            Row {
                height : readOnly ? Global.rowHeight - 3 : Global.rowHeight
                width : parent.width
                Text{
                    width : 140
                    height : parent.height
                    text : qsTr("Maximum Latitude")
                    font.bold: true

                }

                Controls.LatLonCoordinate{
                    id : latmax
                    width : 200
                    isReadOnly: readOnly
                    onMetricChanged: {
                        maxy = metric
                    }
                }
            }
            Row {
                height : readOnly ? Global.rowHeight - 3 : Global.rowHeight
                width : parent.width
                Text{
                    width : 140
                    height : parent.height
                    text : qsTr("Minimum Longitude")
                    font.bold: true
                }

                Controls.LatLonCoordinate{
                    id : lonmin
                    width : 200
                    northsouth: false
                    isReadOnly: readOnly
                    onMetricChanged: {
                        minx = metric
                    }
                }
            }
            Row {
                height : readOnly ? Global.rowHeight - 3 : Global.rowHeight
                width : parent.width
                Text{
                    width : 140
                    height : parent.height
                    text : qsTr("Maximum Longitude")
                    font.bold: true
                }

                Controls.LatLonCoordinate{
                    id :lonmax
                    width : 200
                    northsouth: false
                    isReadOnly: readOnly
                    onMetricChanged: {
                        maxx = metric
                    }
                }
            }

        }
    }

    Component{
        id : metric
        Item {
            anchors.fill: parent
            function setBounds(bb){
                var parts = bb.split(" ")
                if ( parts.length === 4){
                    minxfld.text = parts[0]
                    minyfld.text = parts[1]
                    maxxfld.text = parts[2]
                    maxyfld.text = parts[3]
                }
            }
            Column {

                property string metricbb : minxfld.text + " "  + minyfld.text + " "  + maxxfld.text + " "  + maxyfld.text
                anchors.fill: parent
                spacing : 4
                Row {
                    width : parent.width
                    height : 20
                    spacing : 2
                    Text {
                        text : qsTr("MinX/MinY")
                        width : 140
                        height : 20
                        font.bold: true
                    }
                    TextField{id : minxfld
                        width : 110
                        height : 20
                        text : minx
                        readOnly: csyBoundingBox.readOnly
                        onTextChanged: {
                            minx = text
                        }
                    }
                    TextField{
                        id : minyfld
                        width : 110
                        height : 20
                        text : miny
                        readOnly: csyBoundingBox.readOnly
                        onTextChanged: {
                            miny = text
                        }
                    }
                }
                Row {
                    width : parent.width
                    height : 20
                    spacing : 2
                    Text {
                        text : qsTr("MaxX/MaxY")
                        font.bold: true
                        width : 140
                        height : 20
                    }
                    TextField{
                        id : maxxfld
                        width : 110
                        height : 20
                        text : maxx
                        readOnly: csyBoundingBox.readOnly
                        onTextChanged: {
                            maxx = text
                        }
                    }
                    TextField{
                        id : maxyfld
                        width : 110
                        height : 20
                        text : maxy
                        readOnly: csyBoundingBox.readOnly
                        onTextChanged: {
                            maxy = text
                        }
                    }
                }
            }
        }
    }
    Loader {
        id : valuepairs
        width : parent.width
        height : 40
        sourceComponent : islatlon ? latlon : metric
    }
}

