import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls

Row {
    id : row
    width : childrenRect.width
    height : 25

    Controls.ToolButton{
        iconSource: "../../images/save.png"
        tooltip: qsTr("Saves the model in the current active catalog(if possible)")

        onClicked: {
            var url = mastercatalog.currentCatalog.url
            if ( url.indexOf("file://") !== 0) {
                return
            }
            modellerDataPane.model.store(url)
        }
    }
    Controls.ToolButton{
        iconSource: "../../images/saveas.png"
    }

}
