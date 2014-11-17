import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../controls" as Controls

Item {
    signal makeForm(string objectid, string name)

    id : operationCatTab
    anchors.fill: parent
    Image {
        anchors.fill: parent
        source : "../images/back2.png"
        opacity : 0.5
        smooth : true
    }

    ScrollView {
        width : parent.width
        height : container.height - appFrame.height - functionBarHeader.height - searchBar.height
        Rectangle {
            // anchors.top: functionBar.bottom
            height : oper1.height  + oper2.height + oper3.height + oper4.height + oper5.height + oper6.height + oper7.height + oper8.height + oper9.height + oper10.height
            width : container.width - 14
            Column {
                id : operationPanels
                anchors.fill: parent
                spacing : 2
                Controls.CollapsiblePanel{
                    id : oper1
                    width : parent.width
                    panelHeight: 250
                    state : "collapsed"
                    titleText : qsTr("Image Processing")
                }
                Controls.CollapsiblePanel{
                    id : oper2
                    width : parent.width
                    panelHeight: 250
                    state : "collapsed"
                    titleText : qsTr("Raster processing")
                }
                Controls.CollapsiblePanel{
                    id : oper3
                    width : parent.width
                    panelHeight: 150
                    state : "collapsed"
                    titleText : qsTr("Rasterization")
                }
                Controls.CollapsiblePanel{
                     id : oper4
                     width : parent.width
                     panelHeight: 150
                     state : "collapsed"
                     titleText : qsTr("SEBS")
                 }
                Controls.CollapsiblePanel{
                    id : oper5
                    width : parent.width
                    panelHeight: 150
                    state : "collapsed"
                    titleText : qsTr("DEM - Hydroprocessing")
                }
                Controls.CollapsiblePanel{
                    id : oper6
                    width : parent.width
                    panelHeight: 150
                    state : "collapsed"
                    titleText : qsTr("Interpolation")
                }
                Controls.CollapsiblePanel{
                    id : oper7
                    width : parent.width
                    panelHeight: 150
                    state : "collapsed"
                    titleText : qsTr("Feature processing")
                }
                Controls.CollapsiblePanel{
                    id : oper8
                    width : parent.width
                    panelHeight: 150
                    state : "collapsed"
                    titleText : qsTr("Geometry")
                }
                Controls.CollapsiblePanel{
                    id : oper9
                    width : parent.width
                    panelHeight: 150
                    state : "collapsed"
                    titleText : qsTr("Satistics")
                }
                Controls.CollapsiblePanel{
                    id : oper10
                    width : parent.width
                    panelHeight: 150
                    state : "collapsed"
                    titleText : qsTr("Table processing")
                }
            }
        }
    }
}
