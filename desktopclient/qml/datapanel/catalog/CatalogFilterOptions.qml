import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import InternalDatabaseModel 1.0
import QtGraphicalEffects 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Rectangle {
    anchors.fill: parent
    anchors.topMargin: 3
    color : catalogSplit.backgroundCatalogColor
    property string iconName : "../images/filter"
    Rectangle {
        id : choices
        anchors.bottom : parent.bottom
        width : 350
        height : parent.height
        border.width: 1
        border.color: Global.edgecolor
        color :  uicontext.lightestColor

        Column {
            height : parent.height
            width : parent.width  - 5
            spacing : 4
            x : 5
            Rectangle {
                id : header
                width : parent.width
                height : 18
                color : uicontext.actionHeaderColor
                Text{
                    text : qsTr("Filters")
                    font.bold: true
                    x : 3
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            CatalogFilterButtons{
                id : objectfilters
            }
            Row {
                width : parent.width
                height : 22
                Text {
                    y : 3
                    width : 100
                    font.bold: true
                    text : qsTr("Standard queries")
                }

                CatalogFilterComboBox{
                    id : catalogfilters
                    width : parent.width - 105

                    Controls.ToolTip{
                        target: catalogfilters
                        text : qsTr("A number of default catalog queries. These fill a catalog with the content as defined in the query ")
                    }
                }
            }
            Controls.ComboxLabelPair{
                id : countryselect
                labelText: qsTr("Country select")
                labelWidth: 100
                width : parent.width - 5
                itemModel: internaldatabase.query("Select * from teritories where type='country' order by name")
                role : "name"
                fontSize: 8

                Controls.ToolTip{
                    target: countryselect
                    text : qsTr("Selects coverages that are within the rectangular envelope of a country. It is not an exact boundry match")
                }

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
                id : regionselect
                labelText: qsTr("Region select")
                labelWidth: 100
                width : parent.width - 5
                itemModel: internaldatabase.query("Select * from teritories where type='region' order by name")
                role : "name"
                fontSize: 8

                Controls.ToolTip{
                    target: regionselect
                    text : qsTr("Selects coverages that are within the envelope of a defined region on earth.")
                }

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
                id : keyselect
                labelText: qsTr("Keyword filters")
                labelWidth: 100
                width : parent.width - 5
                transparentBackgrond : false
                fontSize: 8
                onContentChanged: {
                    currentCatalog.filter("keyword", content)
                }
                Controls.ToolTip{
                    target: keyselect
                    text : qsTr("Comma seperated list with keywords that define the filter for this catalog. Only objects that have these keywords will appear")
                }
            }

            Item { 
                height : 20
                width : parent.width
                Controls.TextEditLabelPair{
                    id : label1
                    labelText: qsTr("Name filter")
                    labelWidth: 100
                    width : parent.width -5
                    transparentBackgrond : false
                    fontSize: 8
                    Controls.ToolTip{
                        target: label1
                        text : qsTr("Selects objects with the string in de edit field in the name")
                    }
                    onContentChanged: {
                        currentCatalog.filter("name", content)
                    }
                }
            }
            Item {
                height : 20
                width : parent.width
                Controls.TextEditLabelPair{
                    id : label2
                    labelText: qsTr("Time filter")
                    labelWidth: 100
                    width : parent.width - 47
                    fontSize: 8
                    transparentBackgrond : false
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
    Rectangle {
        id : mapLabel
        width : 370
        height : 18
        color : uicontext.actionHeaderColor
        anchors.left: choices.right
        anchors.leftMargin: 2
        Text{
            text : qsTr("Spatial Selection")
            font.bold: true
            x : 3
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    Controls.SpatialSelection{
        id : spatselect
        anchors.left: choices.right
        anchors.leftMargin: 6
        anchors.top : mapLabel.bottom
        z : 1
        height : parent.height
        width : 370
        filterTarget: currentCatalog
    }
}

