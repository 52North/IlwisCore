import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import ".." as DataPanel

TabView {
    id : catalogoptions
    height : 25
    width : parent.width
    tabPosition: Qt.BottomEdge
    property string side
    Layout.minimumHeight: 25
    style: DataPanel.ButtonBarTabViewStyle{}

    function tabClicked(index){
        if ( currentIndex === index){
            var tab = getTab(currentIndex)
            if ( tab){
                if ( tab.height <= 60){
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
        CatalogViewOptions{}
    }
    Component {
        id : copyOptions
        ObjectCopyOptions{}
    }
    Component {
        id : deleteOptions
        ObjectDeleteOptions{}
    }
    Component {
        id : refreshOptions
        CatalogRefreshOptions{}
    }

    Component.onCompleted: {
        addTab(qsTr("View"), viewOptions)
        addTab(qsTr("Filter"), filterOptions)
        addTab(qsTr("Copy"), copyOptions)
        addTab(qsTr("Delete"), deleteOptions)
        addTab(qsTr("Refresh"), refreshOptions)
    }


}



