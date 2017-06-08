import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import MasterCatalogModel 1.0
import UIContextModel 1.0
import TabModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    id : scriptArea
    width : parent.width
    state : "maxed"
    property alias scriptText : inputArea.text

    function clear(t){
        if ( t === "h")
            logArea.text = ""
        if ( t === "o")
            outputArea.text = ""
        if ( t === "c")
            inputArea.text = ""
    }

    function executeLines(txt){
          var result = script.run(txt)
          outputArea.append(result )
    }

    function runDelete(){
        var script = inputArea.text
        logArea.append(script)
        executeLines(script)
        inputArea.text = ""
    }

    function runKeep(){
        var script = inputArea.text
        logArea.append(script)
        executeLines(script)
    }
    function runSelected(){
        var script = inputArea.selectedText
        logArea.append(script)
        executeLines(script)
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        Row {
            id : a2
            width : parent.width
            Layout.fillHeight: true
            height : parent.height * 0.7
            // AreaHeader { id : tabheader1;labeltext: "Runnable code"}
            Item {
                id : tabheader1; width : 34; height : parent.height
                AreaHeader { labeltext: "Console"}
            }
            TextArea {
                id : inputArea
                height : parent.height
                width : parent.width - tabheader1.width
                font.pointSize:11
                DropArea {
                    id: canvasDropArea
                    anchors.fill: parent
                    enabled: true
                    onDropped: {
                        var oper = operations.operation(drag.source.ilwisobjectid)
                        if ( oper){
                            inputArea.append(oper.pythonSyntax);
                        }else {
                            var obj = mastercatalog.id2Resource(drag.source.ilwisobjectid)
                            if ( obj){
                                if ( obj.typeName === "script"){
                                   bigthing.changeCatalog("itemid=" + obj.id, "script", obj.url);
                                }else {
                                    var b = inputArea.selectionStart;
                                    var e = inputArea.selectionEnd;
                                     inputArea.remove(b,e);
                                    inputArea.insert(b, "\'" + obj.url + "\'")
                                }
                                obj.suicide()

                            }
                        }

                    }
                }

            }
        }
        Row {
            id :a1
            width : parent.width
            height : 80
            //AreaHeader { id: tabheader0;labeltext: "Output"}
            Item {
                id : tabheader0; width : 34; height : parent.height;
                AreaHeader { labeltext: "Output"}
            }
            TextArea {
                id : outputArea
                height :   parent.height
                width : parent.width - tabheader2.width
                readOnly: true
                style: TextAreaStyle {
                    backgroundColor: uicontext.lightestColor
                }
            }
        }
        Row {
            id : a3
            width : parent.width
            height : 60

            Item {
                id : tabheader2;  width : 34; height : parent.height
                AreaHeader { labeltext: "History"}
            }
            TextArea {
                id : logArea
                height : parent.height
                readOnly: true
                width : parent.width - tabheader2.width
                font.pointSize:11
                style: TextAreaStyle {
                    backgroundColor: uicontext.lightestColor
                }
            }
        }
    }
    states: [
    State {
             name : "sized"
             PropertyChanges { target: scriptArea; height : parent.height - Global.actionBarMaxHeight}
         },
         State {
             name : "maxed"
             PropertyChanges { target: scriptArea; height : parent.height - Global.actionBarMinHeight}
         }
    ]
    transitions: [
        Transition {
            ParallelAnimation {
                NumberAnimation { target : scriptArea; properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
            }
        }
    ]
}
