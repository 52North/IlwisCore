import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    width: catalogoptions.width
    height: catalogoptions.height
    Controls.SpatialSelection{
        id : spatselect

    }
    Controls.ComboxLabelPair{
        id : countryselect
        labelText: qsTr("Country select")
        labelWidth: 100
        width : spatselect.width
        anchors.top : spatselect.bottom
        anchors.topMargin: 4

    }
    Controls.ComboxLabelPair{
        labelText: qsTr("Region select")
        labelWidth: 100
        width : spatselect.width
        anchors.top : countryselect.bottom
        anchors.topMargin: 4

    }
    Column {
        height : 200
        width : 220
        anchors.left: spatselect.right
        anchors.leftMargin: 10
        spacing : 4
        Controls.ComboxLabelPair{
            labelText: qsTr("Keyword filters")
            labelWidth: 100
            width : parent.width

        }
        Item {
            height : 20
            width : parent.width
            Controls.TextEditLabelPair{
                id : label1
                labelText: qsTr("Name filter")
                labelWidth: 100
                width : parent.width
            }
            Button{
                anchors.left: label1.right
                anchors.leftMargin: 2
                width : 40
                height : label1.height
                text : qsTr("Apply")
            }
        }
        Item {
            height : 20
            width : parent.width
            Controls.TextEditLabelPair{
                id : label2
                labelText: qsTr("Time filter")
                labelWidth: 100
                width : parent.width
            }
            Button{
                anchors.left: label2.right
                anchors.leftMargin: 2
                width : 40
                height : label2.height
                text : qsTr("Apply")
            }
        }

    }


}

