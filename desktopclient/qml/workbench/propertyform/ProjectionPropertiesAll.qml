import QtQuick 2.0

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ResourceModel 1.0
import IlwisObjectModel 1.0
import ProjectionParameterModel 1.0

import "../../Global.js" as Global

Rectangle {
    function storeData() {
        if ( propertyForm.editable){
        }
    }
    color : uicontext.lightestColor

    width : parent.width
    height : 200
    Item {
        id : otherDefs
        width : parent.width
        height : 100
        Text{
            id : label1
            width : 100
            height : Global.rowHeight
            text : qsTr("Proj4 defintion")
        }
        TextArea{
            anchors.top:label1.bottom
            width : parent.width - 10
            height :70
            text : getProperty("proj4def")
            readOnly: true
        }
    }
    Item {
        id : projectionitems
        anchors.top: otherDefs.bottom
        width : parent.width
        height : 200
        TableView {
            anchors.fill: parent
            model : projectionItems
            TableViewColumn{
                title : qsTr("Projection Parameter");
                role : "name"
                width : 100
                delegate: Component{
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                    }
                }
            }
            TableViewColumn{
                title : qsTr("Value");
                role : "value"
                width : 100
                delegate: Component{
                    Text {
                        text: styleData.value
                        verticalAlignment:Text.AlignVCenter
                        font.pixelSize: 10
                        elide: Text.ElideMiddle
                    }
                }
            }
        }
    }


}


