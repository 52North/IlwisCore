import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0
import UIContextModel 1.0

import "../../Global.js" as Global

FocusScope {
    anchors.fill: parent
    Rectangle {
        anchors.fill :parent
        color : "white"

        Image {
            anchors.fill: parent
            source : "../../images/remoteBig.png"
            fillMode: Image.Stretch
            opacity: 0.07

        }

        ScrollView {
            id : catalogGrid
            property color background1 : "#EAECEE"
            property color background2 : "#FFF9E3"

            anchors.fill : parent
            anchors.margins: 2

            GridView {
                id : itemgrid

                function setSelected(objectid){
                    var ids = ""
                    if ( uicontext.currentKey !== Qt.Key_Control &&  uicontext.currentKey !== Qt.Key_Shift)    {
                        ids = objectid
                        for(var i = 0; i < currentCatalog.resources.length; ++i){
                            if (currentCatalog.resources[i].isSelected && currentCatalog.resources[i].id !== objectid)
                                currentCatalog.resources[i].isSelected=false
                        }
                    }else if ( uicontext.currentKey === Qt.Key_Shift){
                        var startRange = false
                        for(var j = 0; j < currentCatalog.resources.length; ++j){
                            if ( currentCatalog.resources[j].isSelected){
                                startRange = !startRange;
                                ids = ids == "" ? currentCatalog.resources[j].id : ids + "|" +currentCatalog.resources[j].id
                            }else {
                                if ( startRange){
                                    ids = ids + "|" +currentCatalog.resources[j].id
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
                            ids = ids == "" ? currentCatalog.resources[k].id : ids + "|" +currentCatalog.resources[k].id
                          }
                      }
                    }
                    currentCatalog.setSelectedObjects(ids)
                }

                model : setResources()
                delegate: CatalogGridDelegate{}
                cellWidth: 170
                cellHeight: 18
                clip : true
                cacheBuffer: 1000
                flow: GridView.FlowTopToBottom
            }
        }
    }
}



