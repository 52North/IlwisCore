import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls

Row {
    id : row
    width : childrenRect.width
    height : 35

    Controls.ToolButton{
        iconSource: "../../images/save.png"
        tooltip: qsTr("Saves the model in the current active catalog(if possible)")
        height : buttonSize
        width : buttonSize

        onClicked: {
            var url = mastercatalog.currentCatalog.url
            if ( url.indexOf("file://") !== 0) {
                return
            }
            modellerDataPane.model.store(url)
        }
    }
    Controls.ToolButton{
        height : buttonSize
        width : buttonSize
        iconSource: "../../images/saveas.png"
    }

}
