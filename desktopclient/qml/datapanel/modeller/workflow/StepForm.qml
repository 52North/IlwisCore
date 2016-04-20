import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import UIContextModel 1.0
import "../../../workbench" as Bench

import "../../../Global.js" as Global

Column {
    function showObject(objectid){
        var filter
        // we want the object
        var type = mastercatalog.id2type(objectid)
        if ( !type) // unknow type, we can not show it
            return
        var newPanel = null
        var resource = mastercatalog.id2Resource(objectid)
        // object as 'real' data case
        filter = "itemid=" + resource.id
        // try to find a suitable data pane for it
        var side = modellerDataPane.tabmodel.side
        newPanel = datapanesplit.newPanel(filter, resource.typeName,resource.url, side === "left" ? "right" : "left")
        if ( newPanel == null){ // we dont have a seperate pane for it so it is an object with only metadata to show
            mastercatalog.setSelectedObjects(objectid)
            bigthing.getWorkbenchPane("objectproperties","visible");
        }

        if ( resource && newPanel){ // the default parent is too generic. so we choose this panel as its true parent
            resource.makeParent(newPanel) // set the parent correctly as it needs to go as the panels goes and not when the mastercatalog goes(the default parent)
        }
    }


    height : 250
    spacing: 4
    BorderImage {
        id : title
        width: parent.width
        height : 25
        opacity : 1
        source : "../../../images/headerblue2CS1.png"
        border { left: 15; top: 0; right: 15; bottom: 0 }
        smooth : true
        Text {
            text : qsTr("Generated operation values")
            font.pointSize: 11
            x : 5
        }
    }
    TableView {
        width : parent.width
        height :parent.height
        model : modellerDataPane.workflowModel().outputCurrentOperation
        TableViewColumn{
            role : "vertex"
            title : qsTr("Node")
            width : 45
            delegate: Component{
                Text {
                    text: styleData.value
                    x: 4
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 9
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "name"
            title : qsTr("Origin")
            width : 80
            delegate: Component{
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 9
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "type"
            title : qsTr("Type")
            width : 100
            delegate: Component{
                Text {
                    text: uicontext.type2typeName(styleData.value)
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 0
                    elide: Text.ElideMiddle
                }
            }
        }
        rowDelegate: Rectangle {
            id : rowdelegate
            height : 20
            color : styleData.selected ? Global.selectedColor : (styleData.alternate? "#eee" : "#fff")
        }
        onDoubleClicked: {
            if ( currentRow != -1){
                var id = model[currentRow].id
                if ( id !== -1)
                    showObject(model[currentRow].id)
            }
        }
    }


}
