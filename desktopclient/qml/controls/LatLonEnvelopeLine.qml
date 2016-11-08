import QtQuick 2.0
import "../Global.js" as Global

Row {
    property string envelope
    property bool readOnly : true
    property string label : ""
    property int labelWidth : 0
    property int itemWidth : 130

    onEnvelopeChanged: {
        setValues(envelope)
    }

    function setValues(bb){
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

    Text {
        id : lbl
        y : 3
        height : Global.rowHeight
        width : labelWidth
        text: label
        font.bold: true
    }

    LatLonCoordinate{
        id : latmin
        width : itemWidth
        isReadOnly: readOnly
        compact: true
    }
    LatLonCoordinate{
        id : lonmin
        width : itemWidth
        isReadOnly: readOnly
        compact: true
        northsouth : false
    }
    LatLonCoordinate{
        id : latmax
        width : itemWidth
        isReadOnly: readOnly
        compact: true
    }
    LatLonCoordinate{
        id : lonmax
        width : itemWidth
        isReadOnly: readOnly
        compact: true
        northsouth : false
    }
    Component.onCompleted: {
        setValues(envelope)
    }
}
