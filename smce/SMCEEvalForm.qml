import QtQuick 2.0
import LayersView 1.0
import "../../../qml/Global.js" as Global
import "../../../qml/controls" as Controls

Item {
    width: parent.width
    height: parent.height
    Row {
       Column {
            id: nodecolumn
            width : parent.parent.width / 3
            Rectangle {
                id : nodedetaillabel
                height : 18
                width: parent.width
                color : Global.palegreen
                Text{
                    text : qsTr("Selected node")
                    font.bold : true
                    x : 5
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            Rectangle {
                id : nodedetail
                width: parent.width
                height: parent.height - 18

                color : Global.palegreen

                Text{
                    id: inpindicatortext
                    text : qsTr("Input indicator:") + qsTr("map name to be read")
                    font.bold : false
                    x : 5
                }

                Controls.TextEditLabelPair{
                                id : weightfield
                                labelWidth: 80
                                labelText: qsTr("Weight")
                                transparentBackgrond: false
                                anchors.top: inpindicatortext.bottom
                                regexvalidator: /^-?\d*(\.\d*)?$/
                            }
            }
        }

        Column {
            id: standardizationcolumn
            width : parent.parent.width /3
            height: parent.height
            anchors.left: nodecolumn.right

            Rectangle {
                id : standardizationlabel
                height : 18
                width: parent.width
                color : Global.palegreen
                Text{
                    text : qsTr("Standardization")
                    font.bold : true
                    //x : 5
                    //anchors.verticalCenter: parent.verticalCenter
                }
            }


            Rectangle {
                id : standardizationpanel
                color : Global.palegreen
                border.color: Global.darkgreen
                //anchors.fill: parent
                height : parent.height - 18
                width: parent.width


                //SMCEClassStandardizationTable {
                //    id: classstandardizationtable
                //    height: parent.height
                //}

                /*SMCEClassStandardization {
                    id: classstandardization
                    height: parent.height
                }*/

                Loader {
                    id : stdEditor
                    width : parent.width
                    height : parent.height
                }

            }

            Component.onCompleted: {
                // if ( input indicator is class raster)
                    //stdEditor.setSource("SMCEClassStandardization.qml")
                // else
                stdEditor.setSource("SMCEGraphStandardization.qml")
            }
        }

       Column {
            id: previewcolumn
            width : parent.parent.width / 3
            anchors.left: standardizationcolumn.right

            Rectangle {
                id : previewlabel
                width : parent.width
                height : 18
                color : Global.palegreen
                Text{
                    text : qsTr("Preview")
                    font.bold : true
                    x : 5
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Rectangle {
                id : preview
                width : parent.width
                height : parent.height - 18
                color : Global.palegreen
            }
        }
    }
}
