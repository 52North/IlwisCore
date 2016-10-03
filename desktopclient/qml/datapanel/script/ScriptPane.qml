import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import MasterCatalogModel 1.0
import UIContextModel 1.0
import ScriptModel 1.0
import TabModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    id : scriptView
    width : parent.width
    height : parent.height
    property ScriptModel script : uicontext.scriptModel("", scriptView)

    onScriptChanged: {
        if ( script){
            if ( tabmodel){
                scriptArea.scriptText = script.text
                tabmodel.displayName = script.displayName
            }
        }
    }

    property string panelType : "scriptpanel"
    property var createParameters : []
    property TabModel tabmodel

    function addDataSource(filter, sourceName, sourceType){
        var ids = mastercatalog.select(filter,"")
        if (ids && ids.length === 1){
            var obj = uicontext.scriptModel(ids[0], scriptView)
            if ( obj){
                scriptArea.scriptText = obj.text;
                script = obj;
                return obj.id
            }
        }
        return "0"
    }
    SplitView {
        height : parent.height
        width : parent.width
        orientation: Qt.Vertical
        ScriptArea{
            id : scriptArea
        }
        ScriptActions{

        }
    }
}
