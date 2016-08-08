import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import IlwisObjectModel 1.0

import "../../Global.js" as Global

Rectangle {
    id : floatingRect
    function setObject(id){
        ilwisobject = null
        ilwisobject = mastercatalog.id2object(id,floatingRect)
    }
    clip : true

    property var ilwisobject;
    width : 275
    height : 350
    color : Global.alternatecolor4
    border.width: 1
    opacity: 0.9
    z : 1
    ListView {
        anchors.fill: parent
        model : ilwisobject ? ilwisobject.quickProps : null
        delegate: Row {
            width : parent.width - 4
            height : 15
            x: 4
            Text {

                text : modelData.split(":")[0]
                height : 15
                width : 75
            }
            Text {
                text : modelData.split(":")[1]
                height : 15
                width : parent.width - 75
                elide : Text.ElideMiddle
            }
        }
    }
}
