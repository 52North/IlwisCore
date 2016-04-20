import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0


Text {
    property bool useDefaultRange : true

    function fromRange(rangedef){
        if ( rangedef === "")
            return "";
        var index = rangedef.indexOf(":")
        var itemstring = rangedef.substring(index + 1)
        var items = itemstring.split("|");
        if ( items.length === 2){
            return "min: " + format(items[0]) + " max: " + format(items[1]);
        }else if ( items.length === 3)
            return "min: " + format(items[0]) + " max: " + format(items[1]) + " resolution: " + format(items[2])
        return ""
    }
    height: 20
    text : fromRange(rangeDefinition(useDefaultRange, false,""))
    Layout.minimumWidth: 150

    function format(value) {
        if (isNaN(value))
            return value;
        return parseFloat(value).toFixed(3);
    }
}



