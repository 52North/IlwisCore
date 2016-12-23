import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0

ToolBar{
    id : smcetools
    width : parent.width
    height : 31

    property var smceCatalog

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            return ""

        var iconP = "../../../qml/images/" + name
        return iconP
    }
    Row {
        width : parent.width
        height: 25
        /*Loader {
            source : "../../../qml/datapanel/modeller/GenericTools.qml"
            height : 25
            onLoaded: {
                width : item.width
            }
        }

        Button {
            height : 25
            width : 25
            Image {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                source : iconsource("data_folder.png")
            }

        }*/
        /*Button {
            height : 25
            width : 25
           text: "Expand all"
           onClicked: {
           }
        }*/

        Button {
            height : 25
            width : 25
            //text: "Generate"
            Image {
               anchors.verticalCenter: parent.verticalCenter
               anchors.horizontalCenter: parent.horizontalCenter

               source : iconsource("arrowright.png")
            }
            onClicked: {
                console.log("Generate button clicked...")
                //newTab = datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
                //var smceCatalog = mastercatalog.newCatalog("file:///F:/testdata2/BFD/Final_Data/SMCE_Greenbelt_submission_2711/sub","")
                //mastercatalog.currentCatalog = smceCatalog
                //smceCatalog.scanContainer(false, false)
                var resources = smceCatalog.resources
                console.log("resources=" + resources)
                var sz = resources.length
                console.log("start, length=" + sz)
                for(var j = 0; j < sz; ++j) {
                  console.log ("name=" +resources[j].name + " id=" + resources[j].id)
                  if (resources[j].name == "Coastal_stabilization_reclamation_prioirty_sub.mpr") {
                      resource = resources[j]
                      filter = "itemid=" + resources.id
                      newTab = datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
                      break;
                  }
                }
            }
        }
    }

    Component.onCompleted: {
        //newTab = datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
        //var
        smceCatalog = mastercatalog.newCatalog("file:///F:/testdata2/BFD/Final_Data/SMCE_Greenbelt_submission_2711/sub","")
        //mastercatalog.currentCatalog = smceCatalog
        //smceCatalog.scanContainer(false, false)
        var resources = smceCatalog.resources
        var sz = resources.length
        console.log("start, length=" + sz)
        /*for(var j = 0; j < sz; ++j) {
          console.log ("name=" +resources[j].name + " id=" + resources[j].id)
          if (resources[j].name == "Coastal_stabilization_reclamation_prioirty_sub.mpr") {
              resource = resources[j]
              filter = "itemid=" + resources.id
              newTab = datapanesplit.newPanel(filter, resource.typeName,resource.url,"other")
              break;
          }
        }*/
        console.log("end")
    }
}
