import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MessageModel 1.0
import ResourceModel 1.0
import QtQuick 2.0
import "catalog" as Catalog



Rectangle {

    id: centerItem
    color: background4
    height : parent.height - 16
    width : bigthing.width - buttonB.width - infoP.width - 5

    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name

         var iconP = "../images/" + name
         return iconP

     }

    ToolBar{
        id : tabtools
        width : parent.width
        height : 22
        Button {
            height : 20
            width : 20
            id : addSplit
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 2
            Image {
                anchors.fill: parent
                source : iconSource("plus.png")
                fillMode: Image.PreserveAspectFit
            }
        }
        Button {
            height : 18
            width : 18
            id : removeSplit
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: addSplit.left
            anchors.rightMargin: 4
            Image {
                anchors.fill: parent
                source : iconSource("minus.png")
                fillMode: Image.PreserveAspectFit
            }
        }

    }

    TabView {
        anchors.top : tabtools.bottom
        width : parent.width
        anchors.bottom: parent.bottom

        id : tabs
            function showObject(objectid){
                 var component = Qt.createComponent("visualization/Visualize.qml")
                var resource = mastercatalog.id2Resource(objectid)
                if ( resource !== null){
                    var name = resource.displayName
                    var blocksize = 24 / 2;
                    if ( name.length > 15){
                        var part1 = name.substr(0,blocksize)
                        var part2 = name.substr( name.length - blocksize)
                        name = part1 + "..." + part2
                    }
                    var tab = addTab(name,component)
                    currentIndex++
                    transitionInfoPane("Visualization.qml")
                }
            }

            function objectSelected(objectid){

            }

           Tab {
            id : catalog_0
            title : "Catalog"
            Catalog.CatalogPanel{
            }
        }
    }
}
