import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import OperationModel 1.0
import "../../../Global.js" as Global

JunctionItem {
    color : uicontext.middleColor
    property string type : "rangejunctionitem"

    onXChanged: {
        changeBox()
        wfCanvas.canvasValid = false
    }
    onYChanged: {
        changeBox()
        wfCanvas.canvasValid = false
    }
    onWidthChanged: {
        changeBox()
    }
    onHeightChanged: {
        changeBox()
    }

    function attachFlow(target, attachRectTo){
        if ( currentItem === target)
            return false
        var fromIndex = 0
        var toIndex = 0
        var flowPoints = { "fromParameterIndex" : fromIndex, "toParameterIndex" : toIndex};
        currentItem.setFlow(target, attachRectTo, flowPoints,-1)

        wfCanvas.canvasValid = false
    }

    function parameterNames(typefilter, isOutput){
        var names = []
        names.push("dummy")
        return  names
    }
}
