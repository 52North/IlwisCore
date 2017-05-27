import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../controls" as Controls
import "../../Global.js" as Global

Rectangle {
    anchors.fill: parent
    color : catalogSplit.backgroundCatalogColor
    property alias useDropFormat : convert.checked
    property string iconName : "../images/copy"

    function getFormatString(requestedType) {
        return conversionFormats.getFormatString(requestedType)
    }

    Rectangle {
        width: 320
        height: parent.height - 3
        border.width: 1
        border.color: Global.edgecolor
        color :  uicontext.lightestColor
        enabled: currentCatalog ? (currentCatalog.url != "ilwis://internalcatalog" && currentCatalog.url.indexOf("ilwis://system") == -1) : false

        Column {
            anchors.fill: parent
            anchors.leftMargin: 4
            spacing : 4

            CheckBox{
                id : convert
                height : Global.rowHeight
                width : 120
                text: qsTr("Convert on drop")
            }

            Text {
                height : 25
                width : 120
                text : qsTr("Drop conversion formats")
                font.bold: true
                font.pointSize: 10

            }
            DropConversionFormats{
                id : conversionFormats
            }
        }
    }
}

