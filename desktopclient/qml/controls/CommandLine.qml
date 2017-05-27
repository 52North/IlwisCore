import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global
import ConsoleScriptModel 1.0
import UIContextModel 1.0

Rectangle {
    id : commLine;
    color : uicontext.darkColor
    objectName : "mainwindow__commandline"
    z : 3
    property string arrowtype : "arrowdown.png"

    function executeLines(txt){
          var result = uicontext.consoleScript(0).run(txt)
          outputArea.append(result )
    }


    Rectangle {
        id: t3
        anchors.leftMargin: 5
        width : parent.width - titleArrow.height - 10
        height : parent.height
        color : uicontext.darkestColor

        SplitView {
            id : editparts
            y : 4
            width: parent.width
            height: parent.height - 25
            orientation: Qt.Horizontal

            SplitView {
                width : parent.width - parent.width * 0.333
                height : parent.height
                orientation: Qt.Vertical
                TextArea {
                    id : oldCode
                    height :   commLine.state == "collapsed" ? 0 : 40
                    width : parent.width
                    readOnly: true
                    style: TextAreaStyle {
                         backgroundColor: uicontext.lightestColor
                     }
                }
                TextArea {
                    id : inputArea
                    width : parent.width
                    height : parent.height
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
                                    var b = inputArea.selectionStart;
                                    var e = inputArea.selectionEnd;
                                    inputArea.remove(b,e);
                                    inputArea.insert(b, obj.name)
                                    obj.suicide()
                                }
                            }

                        }
                    }

                }
            }
            TextArea {
                id : outputArea
                Layout.fillWidth: true
                height : parent.height
                readOnly: true
                font.pointSize:11
                style: TextAreaStyle {
                     backgroundColor: uicontext.lightestColor
                 }

            }

        }
        Row {
            id : inputButtons
            anchors.top: editparts.bottom
            width : inputArea.width
            height : 25
            Button{
                width: 100
                height : 20
                text : qsTr("Execute")
                onClicked: {
                    var script = inputArea.text
                    oldCode.append(script)
                    executeLines(script)
                    inputArea.text = ""

                }
            }
            Button{
                width: 100
                height : 20
                text : qsTr("Save")
            }
            Button{
                width: 100
                height : 20
                text : qsTr("Load")
            }
        }
        Button {
            width : 100
            height : 20
            text : qsTr("Clear")
            anchors.top : editparts.bottom
            anchors.left: inputButtons.right
            onClicked: {
                outputArea.text = ""
            }
        }

    }
    Rectangle {
        id: titleArrow
        width:  commLine.state == "collapsed" ? 15 : 2
        height:  15
        anchors.left: t3.right
        anchors.leftMargin: commLine.state == "collapsed" ? 2 : 10
        color : commLine.state == "collapsed" ? "#256E4D" : "white"
        border.width: 2
        border.color : "white"
        radius : 3
        y : 8


    }
    MouseArea{
        width: 15
        height:  15
        anchors.left: t3.right
        anchors.leftMargin: 2
        y : 4
        onClicked: {
            if (commLine.state == "collapsed")
            {
                commLine.state = "open"
                topArea.state = "open"
            }
            else
            {
                commLine.state = "collapsed"
                topArea.state = "collapsed"
            }
        }
    }

    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: titleArrow
                rotation: 0
            }
        },
        State{
            name: "open"
            PropertyChanges {
                target: titleArrow
                rotation: 90
            }
        }

    ]

    transitions: [
        Transition {
            NumberAnimation { target: titleArrow; property: "rotation"; duration: 300 }
        }
    ]
    Component.onCompleted:  {
        state = "collapsed"
        topArea.state = "collapsed"
    }
}
