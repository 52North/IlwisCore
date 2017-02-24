import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0

ToolBar{
    id : smcetools
    width : parent.width
    height : 31

    property var smceCatalog

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            return ""

        var iconP = "../../../qml/images/" + name
        return iconP
    }
    Row {
        width : parent.width
        height: 25
        /*Loader {
            source : "../../../qml/datapanel/modeller/GenericTools.qml"
            height : 25
            onLoaded: {
                width : item.width
            }
        }

        Button {
            height : 25
            width : 25
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource("data_folder.png")
            }

        }*/
        /*Button {
            height : 25
            width : 25
           text: "Expand all"
           onClicked: {
           }
        }*/

        Button {
            height : 25
            width : 25
            //text: "Generate"
            Image {
               anchors.verticalCenter: parent.verticalCenter
               anchors.horizontalCenter: parent.horizontalCenter

               source : iconsource("arrowright.png")
            }
            onClicked: {
                var analysis = modellerDataPane.model.analysisPattern(0)
                var input = {
                    inputFilename1 : "dummy"
                }
                var output = {
                    outputFilename1 : "dummy"
                }

                analysis.execute(input, output)
                openMap("Greenbelt_development_priority_sub.mpr")
            }
        }
    }
}
