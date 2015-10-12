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

    function setBoundingBox(bb){
         valuepairs.item.setBounds(bb)
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
            }

            anchors.fill: parent
            property string metricbb : lonmin.metric + " " + latmin.metric + " " + lonmax.metric + " " + latmin.metric
            Row {
                height : Global.rowHeight
                width : parent.width
                Text{
                    width : 120
                    height : parent.height
                    text : qsTr("Minimum Latitude")
                    font.bold: true
                }

                Controls.LatLonCoordinate{
                    id : latmin
                    width : 200
                    onMetricChanged: {
                        miny = metric
                    }
                }
            }
            Row {
                height : Global.rowHeight
                width : parent.width
                Text{
                    width : 120
                    height : parent.height
                    text : qsTr("Maximum Latitude")
                    font.bold: true

                }

                Controls.LatLonCoordinate{
                    id : latmax
                    width : 200
                    onMetricChanged: {
                        maxy = metric
                    }
                }
            }
            Row {
                height : Global.rowHeight
                width : parent.width
                Text{
                    width : 120
                    height : parent.height
                    text : qsTr("Minimum Longitude")
                    font.bold: true
                }

                Controls.LatLonCoordinate{
                    id : lonmin
                    width : 200
                    northsouth: false
                    onMetricChanged: {
                        minx = metric
                    }
                }
            }
            Row {
                height : Global.rowHeight
                width : parent.width
                Text{
                    width : 120
                    height : parent.height
                    text : qsTr("Maximum Longitude")
                    font.bold: true
                }

                Controls.LatLonCoordinate{
                    id :lonmax
                    width : 200
                    northsouth: false
                    onMetricChanged: {
                        miny = metric
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
                    minyfld.text = parts[0]
                    maxyfld.text = parts[1]
                    minxfld.text = parts[2]
                    maxxfld.text = parts[3]
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
                        width : 120
                        height : 20
                        font.bold: true
                    }
                    TextField{id : minxfld
                        width : 110
                        height : 20
                        text : minx
                        onTextChanged: {
                            minx = text
                        }
                    }
                    TextField{
                        id : minyfld
                        width : 110
                        height : 20
                        text : miny
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
                        width : 120
                        height : 20
                    }
                    TextField{
                        id : maxxfld
                        width : 110
                        height : 20
                        text : maxx
                        onTextChanged: {
                            maxx = text
                        }
                    }
                    TextField{
                        id : maxyfld
                        width : 110
                        height : 20
                        text : maxy
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

