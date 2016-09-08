import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../Global.js" as Global
import ConsoleLineModel 1.0
import ConsoleScriptModel 1.0
import UIContextModel 1.0

Rectangle {
    id : commLine;
    color : "#256E4D"
    objectName : "mainwindow__commandline"
    z : 3
    property string arrowtype : "arrowdown.png"

    function setText(line) {
        if (  uicontext.consoleScript(0).lines.length > 0 && line < uicontext.consoleScript(0).lines.length) {
            return uicontext.consoleScript(0).lines[line].content
        }
        return ""

    }
    function setFirstLine(line){
        uicontext.consoleScript(0).addLine(line, true)
        firstLine.consoletext = lastLine()
        uicontext.consoleScript(0).addLine("", false)
        consolearea.model =uicontext.consoleScript(0).lines
        consolearea.currentIndex = uicontext.consoleScript(0).lines.length - 1
        consolearea.currentItem.editFocus = true
        consolearea.positionViewAtEnd()

        return line;
    }

    function setNum(line){
        if (  uicontext.consoleScript(0).lines.length > 0) {
            return uicontext.consoleScript(0).lines[uicontext.consoleScript(0).lines.length - 1].lineNumber
        }
        return ""
    }
    function lastLine(){
        if (uicontext.consoleScript(0).lines.length > 0)
            return uicontext.consoleScript(0).lines[uicontext.consoleScript(0).lines.length - 1].content
        return ""
    }

    function executeLine(lineIndex, txt){
        if ( lineIndex === -1 || lineIndex === uicontext.consoleScript(0).lines.length - 1){
            var result = uicontext.consoleScript(0).run(txt)
            if ( result !== ""){
                uicontext.consoleScript(0).addLine(result, true)
            }
            firstLine.consoletext = lastLine()
            uicontext.consoleScript(0).addLine("", false)
            consolearea.model =uicontext.consoleScript(0).lines
            consolearea.currentIndex = uicontext.consoleScript(0).lines.length - 1
            consolearea.currentItem.editFocus = true
            consolearea.positionViewAtEnd()
        }
    }

    Connections {
        target: firstLine
        onRunLine:{
            executeLine(lineIndex, txt)
        }
    }

    ConsoleLineDelegate{
        id : firstLine
        width : parent.width - titleArrow.height - 10
        consoletext: lastLine()
        line : setNum(0)
        listindex: 100000
        asMainEdit : true
        y : 6
    }
    Rectangle {
        id: t3
        anchors.leftMargin: 5
        anchors.top : firstLine.top
        width : parent.width - titleArrow.height - 10
        height : 25
        opacity : 0
        color : "#256E4D"


        ScrollView{
            anchors.fill: parent
            ListView {
                id : consolearea
                anchors.fill: parent
                clip : true
                model : uicontext.consoleScript(0).lines
                delegate : ConsoleLineDelegate{
                    height : 25
                    onRunLine: {
                        executeLine(lineIndex, txt)
                    }
                }
            }
        }
    }
    Rectangle {
        id: titleArrow
        width:  commLine.state == "collapsed" ? 15 : 2
        height:  15
        anchors.left: firstLine.right
        anchors.leftMargin: commLine.state == "collapsed" ? 2 : 10
        color : commLine.state == "collapsed" ? "#256E4D" : "white"
        border.width: 2
        border.color : "white"
        radius : 3
        y : 11


    }
    MouseArea{
        width: 15
        height:  15
        anchors.left: firstLine.right
        anchors.leftMargin: 2
        y : 6
        onClicked: {
            if (commLine.state == "collapsed")
            {
                commLine.state = "open"
            }
            else
            {
                commLine.state = "collapsed"
            }
        }
    }

    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: t3
                height: 0
            }
            PropertyChanges {
                target: t3
                opacity: 0
            }
            PropertyChanges {
                target: titleArrow
                rotation: 0
            }
            PropertyChanges {
                target: commLine
                height : 35 + 10
            }

        },
        State{
            name: "open"
            PropertyChanges {
                target: t3
                height: 150
            }
            PropertyChanges {
                target: t3
                opacity: 1
            }
            PropertyChanges {
                target: titleArrow
                rotation: 90
            }
            PropertyChanges {
                target: commLine
                height : 160
            }
            PropertyChanges {
                target: consolearea
                currentIndex: uicontext.consoleScript(0).lines.length - 1
            }
        }

    ]

    transitions: [
        Transition {
            NumberAnimation { target: t3; property: "height"; duration: 300 }
            NumberAnimation { target: commLine; property: "height"; duration: 300 }
            NumberAnimation { target: t3; property: "opacity"; duration: 300 }
            NumberAnimation { target: titleArrow; property: "rotation"; duration: 300 }
        }
    ]
    Component.onCompleted:  {
        state = "collapsed"
    }
}
