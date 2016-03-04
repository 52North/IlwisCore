import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import InternalDatabaseModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    width: catalogoptions.width
    height: catalogoptions.height

    Column {
        id : choices
        height : 280
        width : 320
        spacing : 4
        CatalogFilterButtons{
            id : objectfilters
        }
        Row {
            width : parent.width + 40
            height : 22
            Text {
                y : 3
                width : 100
                font.bold: true
                text : qsTr("Standard queries")
            }

            CatalogFilterComboBox{
                id : catalogfilters
                width : parent.width - 140
            }
        }
        Controls.ComboxLabelPair{
            id : countryselect
            labelText: qsTr("Country select")
            labelWidth: 100
            width : parent.width
            itemModel: internaldatabase.query("Select * from teritories where type='teritory' or type='all'order by name")
            role : "name"
            Component.onCompleted: {
                currentIndex = itemModel.count - 4
            }
            onCurrentIndexChanged: {
                if ( currentIndex >= 0) {
                    var teritory = itemModel[currentIndex]
                    if ( teritory.code !== "WORLD"){
                        var envelope = teritory.longmin + " " + teritory.latmin + " " + teritory.longmax + " " + teritory.latmax
                        currentCatalog.spatialFilter = envelope
                        spatselect.currentEnvelope = envelope
                    }else{
                        currentIndex = -1
                        spatselect.currentEnvelope = "entiremap"
                    }
                }
            }

        }
        Controls.ComboxLabelPair{
            labelText: qsTr("Region select")
            labelWidth: 100
            width : parent.width
            itemModel: internaldatabase.query("Select * from teritories where type='region' or type='all' order by name")
            role : "name"
            Component.onCompleted: {
                currentIndex = itemModel.count - 1
            }
            onCurrentIndexChanged: {
                if ( currentIndex >= 0) {
                    var teritory = itemModel[currentIndex]
                    if ( teritory.code !== "WORLD"){
                        var envelope = teritory.longmin + " " + teritory.latmin + " " + teritory.longmax + " " + teritory.latmax
                        currentCatalog.spatialFilter = envelope
                        spatselect.currentEnvelope = envelope
                    }else{
                         currentIndex = -1
                        spatselect.currentEnvelope = "entiremap"
                    }
                }
            }
        }
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
                width : parent.width - 40
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
                width : parent.width - 40
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
    Controls.SpatialSelection{
        id : spatselect
        showState: "open"
        anchors.left: choices.right
        anchors.leftMargin: 10
    }


}

