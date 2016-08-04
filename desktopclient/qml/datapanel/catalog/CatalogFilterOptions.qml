import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import InternalDatabaseModel 1.0
import QtGraphicalEffects 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    anchors.fill: parent
    anchors.topMargin: 3
    Rectangle {
        id : choices
        anchors.bottom : parent.bottom
        width : 350 * Global.uiScale
        height : parent.height
        border.width: 1
        border.color: Global.edgecolor
        color :  Global.actionItemColor


        Column {
            height : parent.height
            width : parent.width  - 5 * Global.uiScale
            spacing : 4 * Global.uiScale
            x : 5 * Global.uiScale
            CatalogFilterButtons{
                id : objectfilters
            }
            Row {
                width : parent.width
                height : 22* Global.uiScale
                Text {
                    y : 3
                    width : 100* Global.uiScale
                    font.bold: true
                    text : qsTr("Standard queries")
                }

                CatalogFilterComboBox{
                    id : catalogfilters
                    width : parent.width - 105* Global.uiScale
                }
            }
            Controls.ComboxLabelPair{
                id : countryselect
                labelText: qsTr("Country select")
                labelWidth: 100* Global.uiScale
                width : parent.width - 5* Global.uiScale
                itemModel: internaldatabase.query("Select * from teritories where type='country' order by name")
                role : "name"
                fontSize: 8 * Global.uiScale
                Component.onCompleted: {
                    currentIndex = itemModel.count - 4
                }
                onCurrentIndexChanged: {
                    if ( currentIndex >= 0) {
                        var teritory = itemModel[currentIndex]
                        if ( teritory.code !== "world"){
                            var dx = Math.abs(teritory.longmin - teritory.longmax) * 0.05
                            var dy = Math.abs(teritory.latmin - teritory.latmax) * 0.05
                            var envelope = (teritory.longmin - dx) + " " + (teritory.latmin -dy) + " " +
                                    (teritory.longmax + dx) + " " + (teritory.latmax + dy)
                            currentCatalog.filter("spatial", envelope)
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
                labelWidth: 100* Global.uiScale
                width : parent.width - 5* Global.uiScale
                itemModel: internaldatabase.query("Select * from teritories where type='region' order by name")
                role : "name"
                fontSize: 8 * Global.uiScale
                Component.onCompleted: {
                    currentIndex = itemModel.count - 1
                }
                onCurrentIndexChanged: {
                    if ( currentIndex >= 0) {
                        var teritory = itemModel[currentIndex]
                        if ( teritory.code !== "world"){
                            var dx = Math.abs(teritory.longmin - teritory.longmax) * 0.05
                            var dy = Math.abs(teritory.latmin - teritory.latmax) * 0.05
                            var envelope = (teritory.longmin - dx) + " " + (teritory.latmin -dy) + " " +
                                    (teritory.longmax + dx) + " " + (teritory.latmax + dy)
                            currentCatalog.filter("spatial", envelope)
                            spatselect.currentEnvelope = envelope
                        }else{
                            currentIndex = -1
                            spatselect.currentEnvelope = "entiremap"
                        }
                    }
                }
            }
            Controls.TextEditLabelPair{
                labelText: qsTr("Keyword filters")
                labelWidth: 100* Global.uiScale
                width : parent.width - 5
                transparentBackgrond : false
                fontSize: 8 * Global.uiScale
                onContentChanged: {
                    currentCatalog.filter("keyword", content)
                }
            }

            Item { 
                height : 20* Global.uiScale
                width : parent.width
                Controls.TextEditLabelPair{
                    id : label1
                    labelText: qsTr("Name filter")
                    labelWidth: 100* Global.uiScale
                    width : parent.width -5* Global.uiScale
                    transparentBackgrond : false
                    fontSize: 8 * Global.uiScale
                    onContentChanged: {
                        currentCatalog.filter("name", content)
                    }
                }
            }
            Item {
                height : 20* Global.uiScale
                width : parent.width
                Controls.TextEditLabelPair{
                    id : label2
                    labelText: qsTr("Time filter")
                    labelWidth: 100* Global.uiScale
                    width : parent.width - 47* Global.uiScale
                    fontSize: 8 * Global.uiScale
                    transparentBackgrond : false
                }
                Button{
                    scale : Global.uiScale
                    anchors.left: label2.right
                    anchors.leftMargin: 2
                    width : 40
                    height : label2.height
                    text : qsTr("Apply")
                }
            }

        }
    }
    Controls.SpatialSelection{
        id : spatselect
        showState: "open"
        anchors.left: choices.right
        anchors.leftMargin: 6* Global.uiScale
        z : 1
        height : parent.height
        width : 370* Global.uiScale
    }
}

