import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import ".." as DataPanel

TabView {
    id : catalogoptions
    height : Global.actionBarMinHeight
    width : parent.width
    tabPosition: Qt.BottomEdge
    property string side
    Layout.minimumHeight: Global.actionBarMinHeight

    property var tooltips : ["Actions on the selected objects in the active catalog",
        "Filtering of data in the active catalog based on various criteria",
        "Behaviour when a file from another catalog is dropped on the this catalog",
        "Various options for refreshing and updating the current catalog"];

    style: DataPanel.ButtonBarTabViewStyle{}

    function getFormatString(requestedType) {
        getTab(2).active = true
        return getTab(2).item.getFormatString(requestedType)
    }

    function useDropFormat() {
        getTab(2).active = true
        return getTab(2).item.useDropFormat
    }


    function refresh() {
        currentCatalog.scanContainer(true,true)
    }

    function tabClicked(index){
        if ( index !== 3) {
            if ( catalogView.state == ""){
                catalogView.state = "sized"
            }
            else if ( currentIndex === index){
                var tab = getTab(index)
                if ( tab){
                    if ( catalogView.state == "maxed"){
                        catalogView.state = "sized"
                    }else if ( catalogView.state == "sized" ){
                        catalogView.state = "maxed"
                    }
                    else{
                        catalogView.state = "maxed"
                    }
                }

            }else{
                if (catalogView.state == "maxed")
                    catalogView.state = "sized"
            }
        }else
            refresh()

        currentIndex = index
    }
    Component {
        id : filterOptions
        CatalogFilterOptions{}
    }

    Component {
        id : viewOptions
        CatalogViewOptions{}
    }
    Component {
        id : copyOptions
        ObjectCopyOptions{}
    }
    Component {
        id : refreshOptions
        CatalogRefreshOptions{}
    }

    Component.onCompleted: {
        addTab(qsTr("Filter"), filterOptions).active = true
        addTab(qsTr("Selection"), viewOptions).active = true
        addTab(qsTr("Copy"), copyOptions).active = true
        addTab(qsTr("Refresh"), refreshOptions).active = true
    }


}



