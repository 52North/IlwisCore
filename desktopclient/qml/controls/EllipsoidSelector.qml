import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import UIContextModel 1.0
import "../Global.js" as Global
import "../controls" as Controls
import ".." as Base

Item {
    id : createEllipsoid
    height : ellipsoidLabel.height + ellChoice.height
    width : parent.width
    property string name : "Wgs 84"

    Row {
        id : ellChoice
        width : parent.width
        height : 20

        function isPrj(objid){
            var tp = mastercatalog.id2type(objid)
            return (tp === "ellipsoid")
        }
        Text {
            id : ellipsoidLabel
            height : parent.height
            width : 120
            text : qsTr("Ellipsoid")
            font.bold: true
        }

        TextFieldDropArea {
            id : droparea
            canUse: ellChoice.isPrj
            width : ellChoice.width - ellipsoidLabel.width
            height : parent.height
            asName : true
            content : name

            onObjectidChanged: {
                name = droparea.content
            }
        }
    }

    Item {

        states: [
            State { name: "maximized"
                PropertyChanges {
                    target: parmcontainer
                    height : 155
                }
            },
            State {
                name : "minimized"
                PropertyChanges {
                    target: parmcontainer
                    height : 0
                }
            }

        ]
        transitions: [
            Transition {
                NumberAnimation { properties: "height"; duration : 500 ; easing.type: Easing.InOutCubic }
            }
        ]
    }
}



