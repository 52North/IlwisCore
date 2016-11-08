import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import "../Global.js" as Global
import "../controls" as Controls
import ".." as Base

Row {
    width : childrenRect.width
    height : Global.rowHeight
    property string degrees
    property string metric
    property bool northsouth : true
    property bool isChanging: false
    property bool isReadOnly : false
    property bool compact : false
    property string nstring : qsTr("North")
    property string estring : qsTr("East")
    property string wstring : qsTr("West")
    property string sstring : qsTr("South")

    function setCoordinate(crd){
        degreestxt.text = crd
        calcstrings()
    }

    onCompactChanged: {
        if ( compact)    {
            nstring = qsTr("N")
            estring = qsTr("E")
            wstring = qsTr("W")
            sstring = qsTr("S")

        }else{
            nstring = qsTr("North")
            estring = qsTr("East")
            wstring = qsTr("West")
            sstring = qsTr("South")
        }
    }

    spacing : compact ? 2 : 4

    function calcstrings() {
        isChanging = true
        if ( degreestxt.text.indexOf(".") != -1){
            var dgr = Number(degreestxt.text) | 0
            var minutes = parseInt((Number(degreestxt.text) - dgr)*60.0, 10)
            var secs = (Number(degreestxt.text) - dgr - minutes/60.0) * 3600

            degreestxt.text = dgr
            minutestxt.text = Math.abs(minutes)
            secondstxt.text = Math.abs(secs).toFixed(2)
            if ( Number(degreestxt.text < 0)){
                half.checked = false
                degreestxt.text = Math.abs(dgr)
                half.text = northsouth ? sstring : wstring
            }else{
                half.checked = true
                half.text = northsouth ? nstring : estring
            }

        }else {
            if ( degreestxt.text && degreestxt.text !== "" ){
                half.checked = degreestxt.text[0] !== "-"
            }
            if ( !half.checked){
                degreestxt.text = Math.abs(Number(degreestxt.text))
            }

            degrees = degreestxt.text + " ° " + minutestxt.text + " ' " + secondstxt.text + " \""
            metric = Number(degreestxt.text) + Number(minutestxt.text) / 60.0 + Number(secondstxt.text) / 3600.0
            if ( !half.checked){
                degrees = "-" + degrees
                metric = "-" + metric
            }

        }
        secondstxt.cursorPosition = 0
        isChanging = false
    }

    ValidatedTextField{
        id : degreestxt
        width : compact ? 20 : 40
        height : parent.height
        regexvalidator: /^-?(\d*(\.\d*)?)/
        readOnly : isReadOnly


        onTextChanged : {
            if (!isChanging)
                calcstrings()
        }
    }
    Text {
        text : "°"
        width : 7
        height : parent.height
        clip : true
    }
    ValidatedTextField{
        id : minutestxt
        width : compact ? 20 : 40
        height : parent.height
        regexvalidator: /^\+?(0|[1-9]\d*)$/
        readOnly : isReadOnly

        onTextChanged : {
            if (!isChanging)
                calcstrings()
        }
    }
    Text {
        text : "'"
        width : compact ? 5 : 10
        height : parent.height
        clip : true
    }
    ValidatedTextField{
        id : secondstxt
        width : compact ? 35 : 50
        height : parent.height
        regexvalidator: /^\d*(\.\d*)?$/
        readOnly : isReadOnly

        onTextChanged : {
            if (!isChanging)
                calcstrings()
        }
    }
    Text {
        text : "\""
        width : compact ? 5 : 10
        height : parent.height
        clip : true
    }
    Button{
        id : half
        height : 20
        width : compact ? 20 : 40
        checked : true
        enabled: isReadOnly ? false : true
        text : (northsouth ? (checked ? nstring : sstring) : (checked ? estring : wstring))
        onClicked:  {
            checked = !checked
        }
        clip : true
    }
}

