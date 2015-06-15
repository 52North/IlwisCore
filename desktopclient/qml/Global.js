.pragma library
var mainbackgroundcolor = "white";
var edgecolor = "lightgrey"
var alternatecolor1 = "lightgrey"
var alternatecolor2 = "white"
var alternatecolor3 = "#F7F9FC"
var alternatecolor4 = "#eee"
var alternatecolor5 = "#EAECEE"
var headerdark = "steelblue"
var headerlight = "lightsteelblue"
var selectedColor = "#99CCFF"
var rowHeight = 20

function isNumber(n) {
  return !isNaN(parseFloat(n)) && isFinite(n);
}

function calcZoomOutEnvelope(envelope, layers, viewmanager)
{
    if ( envelope !== ""){
        var parts = envelope.split(" ")
        var x1 = parseFloat(parts[0])
        var y1 = parseFloat(parts[1])
        var x2 = parseFloat(parts[3])
        var y2 = parseFloat(parts[4])
        var dx = 0.707 * Math.abs(x1 - x2)
        var dy = 0.707 * Math.abs(y1 - y2)
        var nx1 = (x2 + x1) / 2.0 - dx
        var nx2 = (x2 + x1) / 2.0 + dx
        var ny1 = (y2 + y1) / 2.0 - dy
        var ny2 = (y2 + y1) / 2.0 + dy
        var newenvelope = nx1 + " " + ny1 + " " + nx2 + " " + ny2
        layers.newExtent(newenvelope)
        viewmanager.newZoomExtent(newenvelope)
    }
}
