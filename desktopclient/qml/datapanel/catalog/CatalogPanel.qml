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
    property string panelType : "catalogpanel"
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
        if ( objectid === -1){
            var container = currentCatalog.container
            filter = "container='" + container + "'"
            datapanesplit.changePanel(filter, "catalog", container)
        }else {
            var type = mastercatalog.id2type(objectid)
             var newPanel = null
            if ( !type)
                return
            var resource = mastercatalog.id2Resource(objectid)
            if ( resource.typeName === "catalog" || resource.hasExtendedType("catalog")){
                filter = "container='" + resource.url + "'"
                newPanel = datapanesplit.changePanel(filter, "catalog",resource.url)
            }else {
                filter = "itemid=" + resource.id
                newPanel =datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
                if ( newPanel == null){
                    mastercatalog.setSelectedObjects(objectid)
                    bigthing.getWorkbenchPane("objectproperties","visible");
                }
            }
            if ( resource && newPanel){
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


    CatalogButtonBar{
        id : toolbar
        anchors.top : parent.top
    }
    SplitView {
        anchors.top : toolbar.bottom
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - toolbar.height

        CatalogViews {
            id : catalogView
            height : parent.height - 25
        }
        CatalogActions{
            id : cbuttonBar
        }

    }
}
