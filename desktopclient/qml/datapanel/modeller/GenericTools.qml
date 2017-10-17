import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls

Item {
    id : tools
    height : 35
    width :  workflowOnly ? 0 : 64
    Row {
        id : row
        width : childrenRect.width
        height : parent.height

        Controls.ToolButton{
            iconSource: "../../images/save.png"
            tooltip: qsTr("Saves the model in the current active catalog(if possible)")
            height : 28
            width : 30

            onClicked: {
                var url = mastercatalog.currentCatalog.url
                if ( url.indexOf("file://") !== 0) {
                    url = model.rawUrl
                    if(url.indexOf("file://") !== 0)
                        return
                }
                modellerDataPane.model.store("","")
            }
        }
        Controls.ToolButton{
            height : 28
            width : 30
            iconSource: "../../images/saveas.png"
            onClicked: {
                savestuff2.x = x
                savestuff2.toggle()
            }
        }

    }


}
