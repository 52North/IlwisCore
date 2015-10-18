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
    height : projlabel.height + projchoice.height + parmcontainer.height
    width : parent.width

    function projectionInfo() {
        if ( projnames.currentIndex == -1)
            return null
        var id = projnames.model[currentIndex].split("|")[0]
        return prjparameters.projectionParameters()

    }

//    Text {
//        id : projlabel
//        height : 20
//        width : parent.width
//        text : qsTr("Projections")
//        font.bold: true
//    }

//    Rectangle {
//        id : projlist
//        anchors.top : projlabel.bottom
//        width : parent.width
//        height : 80
//        border.width: 1
//        radius: 3
//        border.color : Global.edgecolor
//        ScrollView{
//            anchors.fill: parent
//            ListView {
//                id : projnames
//                anchors.fill: parent
//                clip:true
//                currentIndex: -1
//                model : mastercatalog.select("type=" + uicontext.typeName2typeId("projection"),"name")
//                highlight : Rectangle { height : 16; width : parent.width; color : Global.selectedColor}
//                delegate {
//                    Text {
//                        width : parent.width
//                        height : 16
//                        text : modelData.split("|")[1]
//                        x : 3
//                        font.pointSize: 9

//                        MouseArea{
//                            anchors.fill : parent
//                            onClicked: {
//                                projnames.currentIndex = index
//                                var ilwobj = mastercatalog.id2object(modelData.split("|")[0],projnames)
//                                if ( ilwobj){
//                                    prjparameters.model = ilwobj.projectionItems;
//                                    parmcontainer.state = "maximized"
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }

    Row {
        id : projchoice
        width : parent.width
        height : 20

        function isPrj(objid){
            var tp = mastercatalog.id2type(objid)
            return (tp === "projection")
        }
        Text {
            id : projlabel
            height : parent.height
            width : 120
            text : qsTr("Projections")
            font.bold: true
        }

        TextFieldDropArea {
            id : droparea
            canUse: projchoice.isPrj
            width : projchoice.width - projlabel.width
            height : parent.height
            asName : true

            onObjectidChanged: {
                var ilwobj = mastercatalog.id2object(objectid,createProjection)
                if ( ilwobj){
                    prjparameters.model = ilwobj.projectionItems;
                    parmcontainer.state = "maximized"
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
            border.width: 1
            border.color: Global.edgecolor
            radius: 3


            Controls.ProjectionParametersEditor{
                id : prjparameters
                width : parent.width
                height : parmborder.height
                anchors.margins: 2
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

