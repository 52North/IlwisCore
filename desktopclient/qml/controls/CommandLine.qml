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
    height : 35
    color : Global.alternatecolor5
    z : 3
    property string arrowtype : "arrowdown.png"

    function setText(line) {
        if (  uicontext.consoleScript(0).lines.length > 0 && line < uicontext.consoleScript(0).lines.length) {
            return uicontext.consoleScript(0).lines[line].content
        }
        return ""

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
        width : parent.width - titleArrow.width
        consoletext: lastLine()
        line : setNum(0)
        listindex: 100000
        asMainEdit : true
    }
    Rectangle {
        id: t3
        anchors.leftMargin: 5
        anchors.top : firstLine.top
        width : parent.width - titleArrow.width
        height : 25
        opacity : 0
        color : Global.alternatecolor2


        ScrollView{
            anchors.fill: parent
            ListView {
                id : consolearea
                anchors.fill: parent
                clip : true
                model : uicontext.consoleScript(0).lines
                delegate : ConsoleLineDelegate{
                    height : 20
                    onRunLine: {
                        executeLine(lineIndex, txt)
                    }
                }
            }
        }
    }
    Image {
        id: titleArrow
        width:  15
        height:  15
        anchors.left: t3.right
        anchors.leftMargin: 2
        y : 5
        source: "../images/" + arrowtype
        MouseArea{
            width: parent.width
            height:  parent.height
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
                height : 35
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
