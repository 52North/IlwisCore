import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import CatalogModel 1.0
import MasterCatalogModel 1.0
import CatalogFilterModel 1.0
import TabModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    property int heightButtons :22
    property CatalogModel currentCatalog
    property TabModel tabmodel
    property string panelType : "catalog"
    id : catalogViews
    width : parent.width
    height : parent.height

    onTabmodelChanged: {
        if ( cbuttonBar && tabmodel)
            cbuttonBar.side = tabmodel.side
    }

    signal catalogChanged()

    function showObject(objectid){
        var filter
        if ( objectid === -1){ // case for  .. one step back
            var container = currentCatalog.container
            filter = "container='" + container + "'"
            datapanesplit.changePanel(filter, "catalog", container)
        }else { // we want the object
            var type = mastercatalog.id2type(objectid)
            if ( !type) // unknow type, we can not show it
                return
            var newPanel = null
            var resource = mastercatalog.id2Resource(objectid)
            if ( resource.typeName === "catalog" || resource.hasExtendedType("catalog")){ // object as container case
                filter = "container='" + resource.url + "'"
                newPanel = datapanesplit.changePanel(filter, "catalog",resource.url)
            }else { // object as 'real' data case
                filter = "itemid=" + resource.id
                // try to find a suitable data pane for it
                newPanel =datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
                if ( newPanel == null){ // we dont have a seperate pane for it so it is an object with only metadata to show
                    mastercatalog.setSelectedObjects(objectid)
                    bigthing.getWorkbenchPane("objectproperties","visible");
                }
            }
            if ( resource && newPanel){ // the default parent is too generic. so we choose this panel as its true parent
                resource.makeParent(newPanel) // set the parent correctly as it needs to go as the panels goes and not when the mastercatalog goes(the default parent)
            }
        }
    }

    function addDataSource(filter, sourceName, sourceType){
        var url = sourceName
        if ( currentCatalog)
            currentCatalog.makeParent(0) // begone though evil creature

        currentCatalog = mastercatalog.newCatalog(url,filter)
        if ( currentCatalog){
            currentCatalog.makeParent(catalogViews)
            mastercatalog.currentCatalog = currentCatalog
        }
        return currentCatalog.id
    }

    function setSelected(objectid){
        mastercatalog.setSelectedObjects("")
        var selectedIds
        if ( uicontext.currentKey !== Qt.Key_Control &&  uicontext.currentKey !== Qt.Key_Shift)    {
            selectedIds = objectid
            for(var i = 0; i < currentCatalog.resources.length; ++i){
                if (currentCatalog.resources[i].isSelected && currentCatalog.resources[i].id !== objectid)
                    currentCatalog.resources[i].isSelected=false
            }
        }else if ( uicontext.currentKey === Qt.Key_Shift){
            var startRange = false
            for(var j = 0; j < currentCatalog.resources.length; ++j){
                if ( currentCatalog.resources[j].isSelected){
                    startRange = !startRange;
                    selectedIds = selectedIds == "" ? currentCatalog.resources[j].id : selectedIds + "|" +currentCatalog.resources[j].id
                }else {
                    if ( startRange){
                        selectedIds = selectedIds + "|" +currentCatalog.resources[j].id
                        currentCatalog.resources[j].isSelected = true
                    }
                    else {
                        currentCatalog.resources[j].isSelected=false
                    }
                }
            }
        } else if ( uicontext.currentKey === Qt.Key_Control){
          for(var k = 0; k < currentCatalog.resources.length; ++k){
              if ( currentCatalog.resources[k].isSelected){
                selectedIds = selectedIds == "" ? currentCatalog.resources[k].id : selectedIds + "|" +currentCatalog.resources[k].id
              }
          }
        }
        mastercatalog.setSelectedObjects(selectedIds)
    }

    function setResources(){
        if ( currentCatalog)
            return currentCatalog.resources
    }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }

    Rectangle{
        id : infobar
        width : parent.width
        height : 20
        color : Global.alternatecolor4
        Text {
            width : parent.width
            height : 16
            text : currentCatalog ? currentCatalog.url : ""
            anchors.centerIn: parent
            color : "#000099"
            opacity: 0.7
        }
    }

    SplitView {
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - infobar.height
        anchors.top : infobar.bottom

        CatalogViews {
            id : catalogView
            height : parent.height - 25
        }
        CatalogActions{
            id : cbuttonBar
        }

    }
}
