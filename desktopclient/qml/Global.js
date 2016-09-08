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
var formBackGround = "#F9F9F9"
var altFormBackGround = "#E6E6E6"
var selectedColor = "#99CCFF"
var selectedTabColor = "#B0C4DE"
var actionItemColor =  "#f2f0e9"
var alternateSelectedTabColor = "#87CEFA"
var notSelectedTabColor = "white"
var uiScale = 1.0
var rowHeight = 20 * uiScale
var actionBarMaxHeight = 280  * uiScale
var actionBarMinHeight = 50  * uiScale
var panningAlong = 1;
var panningReverse = -1;
var darkestgreen = "#256E4D"
var darkgreen = "#2E8D63"
var middlegreen = "#6CBFA1"
var palegreen = "#B9CEBD"
var lightgreen = "#DDE6DE"
var lightestgreen = "#EBF0EC"

function isNumber(n) {
  return !isNaN(parseFloat(n)) && isFinite(n);
}

function calcZoomOutEnvelope(envelope, layers, viewmanager, factor)
{
    if ( envelope !== ""){
        var x1,x2,y1,y2
        var parts = envelope.split(" ")
        if ( parts.length === 6){
            x1 = parseFloat(parts[0])
            y1 = parseFloat(parts[1])
            x2 = parseFloat(parts[3])
            y2 = parseFloat(parts[4])
        } else{
            x1 = parseFloat(parts[0])
            y1 = parseFloat(parts[1])
            x2 = parseFloat(parts[2])
            y2 = parseFloat(parts[3])
        }

        var dx = factor * Math.abs(x1 - x2)
        var dy = factor * Math.abs(y1 - y2)

        var nx1 = (x2 + x1) / 2.0 - dx
        var nx2 = (x2 + x1) / 2.0 + dx
        var ny1 = (y2 + y1) / 2.0 - dy
        var ny2 = (y2 + y1) / 2.0 + dy
        console.debug(x1, x2, nx1, nx2, dx, factor)
        var newenvelope = nx1 + " " + ny1 + " " + nx2 + " " + ny2
        layers.newExtent(newenvelope)
        if (typeof viewmanager.newZoomExtent == 'function')
            viewmanager.newZoomExtent(newenvelope)
    }
}


function createfileUrlFromParts(left, right) {
    if (Qt.platform.os === "windows") {
        return "file:///" + left + right;
    }
    return "file://" + left + right;
}

function urlToLocalPath(path) {
    if (Qt.platform.os === "windows") {
        return path.replace("file:///","")
    }
    return path.replace("file://","")
}
