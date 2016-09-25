import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import ".." as DataPanel

TabView {
    id : scriptoptions
    height : Global.actionBarMinHeight
    width : parent.width
    tabPosition: Qt.BottomEdge
    property string side
    Layout.minimumHeight: Global.actionBarMinHeight

    style: DataPanel.ButtonBarTabViewStyle{}

    function tabClicked(index){
        if ( currentIndex === index){
            var tab = getTab(currentIndex)
            if ( tab){
                if ( scriptArea.state == "maxed"){
                    scriptArea.state = "sized"
                }
                else{
                    scriptArea.state = ""
                    scriptArea.state = "maxed"
                }
            }

        }

        currentIndex = index
    }

    Component {
        id : runOptions
        ScriptRunOptions{}
    }

    Component {
        id : metadOptions
        ScriptMetaDataOptions{}
    }

    Component.onCompleted: {
        addTab(qsTr("Run"), runOptions).active = true
        addTab(qsTr("Metadata"), metadOptions).active = true
    }
}
