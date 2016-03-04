import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import UIContextModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Column {
    width : parent.width
    height : 270
    function code() {
        var index = content.indexOf(":")
        if ( index == -1)
            return content
        var b = content.split(":")[0]
        return b
    }

    property alias content : epsgnumber.content
        spacing: 3
    Controls.TextEditLabelPair {
        id : epsgnumber
        labelText: qsTr("EPSG number")
        labelWidth: 140
        width : parent.width
        height : Global.rowHeight
    }
    Controls.TextEditLabelPair {
        id : filter
        labelText: qsTr("Filter")
        labelWidth: 140
        width : parent.width
        height : Global.rowHeight

        onContentChanged: {
             epsgList.namefilter = content
        }
    }
    Rectangle {
        width : parent.width
        height : 240
        border.width : 1
        border.color : Global.edgecolor

        ScrollView{
            anchors.fill: parent
            ListView{
                id : epsgList
                property string namefilter
                function setFilter(){
                    var filt = "code like 'epsg:%'"
                    if ( namefilter != "")
                        return filt + " and name like '%"+ namefilter + "%'"
                    return filt
                }

                function makeText(txt){
                    var parts = txt.split("|")
                    return parts[2] + ": " + parts[1]
                }
                highlightFollowsCurrentItem: false
                highlight:    Rectangle {
                    width: epsgList.width - 6; height: 14
                    x : 3
                    color: Global.selectedColor; radius: 2
                    y: (epsgList && epsgList.currentItem) ? epsgList.currentItem.y : 0
                }

                anchors.fill: parent
                model : mastercatalog.select(setFilter(),"epsg")
                delegate : Text {
                    width : parent.width
                    height : 16
                    text : epsgList.makeText(modelData)
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if ( index != epsgList.currentIndex){
                                epsgList.currentIndex = index
                                epsgnumber.content = text
                            }
                        }
                    }
                }
            }
        }
    }
}

