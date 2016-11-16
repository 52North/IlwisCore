import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../../controls" as Controls
import "../../../Global.js" as Global

Item {
    id : firstColumn
    width : 320
    height: parent.height

    function refreshList(){
       appNamesList.model = null
       appNamesList.model = modellerDataPane.model.applicationNames(analysisList.comboText)

    }

    Column {
        width : parent.width - 8
        x : 4
        height : parent.height - 8
        y : 4
        spacing: 4
        Row {
            width : parent.width
            height : Global.rowHeight
            Controls.ComboxLabelPair{
                id : analysisList
                width : parent.width - createBut.width
                height : parent.height
                labelWidth: 100
                labelText: qsTr("Analysis names")
                itemModel: modellerDataPane.model ? modellerDataPane.model.analysisNames : null
            }
            Button{
                id : createBut
                width :70
                height:Global.rowHeight
                Image{
                    anchors.centerIn : parent;
                    width : Global.rowHeight - 6;
                    height:Global.rowHeight - 6;
                    source: "../../../images/plus20.png"
                    fillMode: Image.PreserveAspectFit
                }

                onClicked : {
                    createForm.appType = analysisList.comboText
                    createForm.state = "visible"
                }
            }
        }
        Rectangle {
            width : parent.width
            height : parent.height - Global.rowHeight - 10
            border.width: 1
            border.color: Global.edgecolor
            ListView {
                id : appNamesList
                model : modellerDataPane.model ? modellerDataPane.model.applicationNames(analysisList.comboText) : null
                anchors.fill: parent
                anchors.margins: 3
                highlight : Rectangle{width : parent.width;height : 18;color : Global.selectedColor}
                delegate: Text{
                    width : parent.width
                    height : 20
                    text : modelData
                    x : 3
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            appNamesList.currentIndex = index
                            var app = modellerDataPane.model.application(index);
                            if ( app){
                                applicationView.currentApplication = app
                                appv.refreshFormArea(app.panel("form"))
                            }
                        }
                    }
                }
            }
        }
    }

}
