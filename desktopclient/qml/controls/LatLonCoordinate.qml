import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import "../Global.js" as Global
import "../controls" as Controls
import ".." as Base

Row {
    width : parent.width
    height : Global.rowHeight
    property string degrees
    property string metric
    property bool northsouth : true

    function setCoordinate(crd){
        degreestxt.text = crd
    }

    spacing : 5

    function calcstrings() {
        if ( degreestxt.text.indexOf(".") != -1){
            var dgr = Number(degreestxt.text) | 0
            var minutes = parseInt((Number(degreestxt.text) - dgr)*60.0, 10)
            var secs = (Number(degreestxt.text) - dgr - minutes/60.0) * 3600
            degreestxt.text = dgr
            minutestxt.text = minutes
            secondstxt.text = secs
            if ( Number(degreestxt.text < 0)){
                half.checked = false
                half.text = northsouth ? qsTr("South") : qsTr("West")
            }else{
                half.checked = true
                half.text = northsouth ? qsTr("North") : qsTr("East")
            }

        }else {
            if ( degreestxt.text && degreestxt.text !== "" ){
                half.checked = degreestxt.text[0] !== "-"
            }
                degrees = degreestxt.text + " ° " + minutestxt.text + " ' " + secondstxt.text + " \""
                metric = Number(degreestxt.text) + Number(minutestxt.text) / 60.0 + Number(secondstxt.text) / 3600.0

        }
    }

    ValidatedTextField{
        id : degreestxt
        width : 30
        height : parent.height
        regexvalidator: /^-?(\d*(\.\d*)?)/

        onTextChanged : {
            calcstrings()
        }
    }
    Text {
        text : " °"
        width : 10
       height : parent.height
       clip : true
    }
    ValidatedTextField{
        id : minutestxt
        width : 30
        height : parent.height
        regexvalidator: /^\+?(0|[1-9]\d*)$/

        onTextChanged : {
            calcstrings()
        }
    }
    Text {
        text : " '"
        width : 10
       height : parent.height
       clip : true
    }
    ValidatedTextField{
        id : secondstxt
        width : 50
        height : parent.height
        regexvalidator: /^\d*(\.\d*)?$/

        onTextChanged : {
            calcstrings()
        }
    }
    Text {
        text : " \""
        width : 20
        height : parent.height
       clip : true
    }
    Button{
        id : half
        height : 20
        width : 40
        checked : true
        text : (northsouth ? (checked ? qsTr("North") : qsTr("South")) : (checked ? qsTr("East") : qsTr("West")))
        onClicked:  {
            checked = !checked
        }
       clip : true
    }
}

