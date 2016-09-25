import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import MasterCatalogModel 1.0
import ConsoleScriptModel 1.0
import UIContextModel 1.0
import TabModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    id : scriptView
    width : parent.width
    height : parent.height

    property string panelType : "scriptpanel"
    property var createParameters : []
    property TabModel tabmodel

    function addDataSource(filter, sourceName, sourceType){
        var ids = mastercatalog.select(filter,"")
        if (ids && ids.length === 1){
            var obj = mastercatalog.id2object(ids[0], scriptView)
            if ( obj){
                var txt = obj.getProperty("script");
                scriptArea.scriptText = txt
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
