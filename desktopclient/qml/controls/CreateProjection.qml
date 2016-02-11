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
    id : createProjection
    height : projections.height + projchoice.height + parmcontainer.height
    width : parent.width
    property string name : ""

    function projectionInfo() {
        return prjparameters.projectionParameters()

    }


    Row {
        id : projchoice
        width : parent.width
        height : 20

        function isPrj(objid){
            var tp = mastercatalog.id2type(objid)
            return (tp === "projection")
        }
        Controls.SelectProjection{
            id : projections
            width : projchoice.width - 10
            onSelectedObjectidChanged : {
                var ilwobj = mastercatalog.id2object(selectedObjectid,createProjection)
                if ( ilwobj){
                    prjparameters.model = ilwobj.projectionItems;
                    parmcontainer.state = "maximized"
                    createProjection.name = ilwobj.name
                }
            }
        }
    }

    Item {
        id : parmcontainer
        height : 0
        width : parent.width
        anchors.top : projchoice.bottom
        anchors.topMargin: 4
        clip : true
        state : "minmized"
        Text {
            id : parmlabel
            height : 20
            width : parent.width
            text : qsTr("Projection Parameters")
            font.bold: true
        }
        Rectangle {
            id : parmborder
            anchors.top : parmlabel.bottom
            anchors.topMargin: 2
            width : parent.width
            height : parmcontainer.height - 25


            Controls.ProjectionParametersEditor{
                id : prjparameters
                width : parent.width - 4
                height : parmborder.height - 4
                x: 2
                y: 2
            }
        }
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

