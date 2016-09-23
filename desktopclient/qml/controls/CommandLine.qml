import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global
import ConsoleScriptModel 1.0
import UIContextModel 1.0

Rectangle {
    id : commLine;
    color : Global.darkgreen
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
        color : Global.darkestgreen

        SplitView {
            id : editparts
            y : 4
            width: parent.width
            height: parent.height - 25
            orientation: Qt.Horizontal

            Row {
                width : parent.width - parent.width * 0.333
                height : parent.height
                Rectangle {
                    id : execPoint
                    height : parent.height
                    width : 20
                    color : Global.middlegreen
                }
                TextArea {
                    id : inputArea
                    property int endLastExecutionPosition : 0
                    property int lastPoint : 0
                    width : parent.width - execPoint.width
                    height : parent.height
                    onTextChanged: {
                        inputArea.lastPoint = Math.max(lastPoint,inputArea.cursorPosition)
                    }
                }
            }
            TextArea {
                id : outputArea
                Layout.fillWidth: true
                height : parent.height
                readOnly: true
                style: TextAreaStyle {
                     backgroundColor: Global.lightestgreen
                 }

            }

        }
        Row {
            id : inputButtons
            anchors.top: editparts.bottom
            width : inputArea.width + execPoint.width
            height : 25
            Button{
                width: 100
                height : 20
                text : qsTr("Execute")
                onClicked: {
                    var script = inputArea.text.substring(inputArea.endLastExecutionPosition)
                    uicontext.consoleScript(0).countLines(inputArea.text)
                    uicontext.consoleScript(0).findExecutionLine(inputArea.text,inputArea.lastPoint);
                    inputArea.endLastExecutionPosition = inputArea.lastPoint
                    executeLines(script)

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
            Button{
                width: 100
                height : 20
                text : qsTr("Clear")
                onClicked: {
                    inputArea.text = ""
                    inputArea.endLastExecutionPosition = 0
                    inputArea.lastPoint = 0
                }
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
