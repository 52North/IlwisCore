import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Column {
    id : createForm
    state : "invisible"
    height : parent.height
    spacing : 3
    x : 3
    Controls.TextEditLabelPair{
        width : parent.width
        labelWidth: 80
        labelText: qsTr("Name")
        transparentBackgrond: false
    }
    Controls.TextEditLabelPair{
        width : parent.width
        labelWidth: 80
        labelText: qsTr("Long Name")
        transparentBackgrond: false
    }
    Controls.TextEditLabelPair{
        width : parent.width
        labelWidth: 80
        labelText: qsTr("Owner")
        transparentBackgrond: false
    }
    Controls.TextEditLabelPair{
        width : 200
        labelWidth: 80
        labelText: qsTr("Time")
        readOnly: true

    }
    Controls.SpatialSelectionDropDown{
         width : parent.width
         height : 22


    }

    Controls.TextAreaLabelPair{
        width : parent.width
        labelWidth: 80
        height : parent.height - 140
        labelText: qsTr("Description")
    }
    states: [
        State { name: "invisible"

            PropertyChanges {
                target: createForm
                width : 0
            }
        },
        State {
            name : "visible"
            PropertyChanges {
                target: createForm
                width : layerprops.width - firstColumn.width - 3
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "width"; duration : 630 ; easing.type: Easing.InOutCubic }
        }
    ]
}
