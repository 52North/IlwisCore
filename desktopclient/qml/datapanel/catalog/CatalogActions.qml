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
    style: DataPanel.ButtonBarTabViewStyle{}

    function getFormatString(requestedType) {
        getTab(2).active = true
        return getTab(2).item.getFormatString(requestedType)
    }

    function useDropFormat() {
        getTab(2).active = true
        return getTab(2).item.useDropFormat
    }



    function tabClicked(index){
        if ( currentIndex === index){
            var tab = getTab(currentIndex)
            if ( tab){
                if ( catalogView.state == "maxed"){
                    catalogView.state = "sized"
                }
                else{
                    catalogView.state = ""
                    catalogView.state = "maxed"
                }
            }

        }

        currentIndex = index
    }
    Component {
        id : viewOptions
        CatalogViewOptions{}
    }
    Component {
        id : filterOptions
        CatalogFilterOptions{}
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
        addTab(qsTr("Selection"), viewOptions)
        addTab(qsTr("Filter"), filterOptions)
        addTab(qsTr("Copy"), copyOptions)
        addTab(qsTr("Refresh"), refreshOptions)
    }


}



