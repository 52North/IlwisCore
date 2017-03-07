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
    property int buttonSize : 30

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
        Loader {
            source : "../../../qml/datapanel/modeller/GenericTools.qml"
            height : 25
            onLoaded: {
                width : item.width
            }
        }

        Button {
            height : 30
            width : 30
            //text: "Generate"
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource("run20.png")
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
                var fileName = tree.fileName
                if (fileName.indexOf(".mpr", fileName.length - 4) === -1)
                    fileName += ".mpr"
                openMap(fileName)
            }
        }
    }

}
