import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../Global.js" as Global
import "../.." as Base

Item {
    id : inner
    width: parent.width
    x : 5
    y : 5
    height: parent.height -10
    property var columnmodel
    property string iconName : "../images/stat"


    TableView{
        model : columnmodel
        anchors.fill :parent
        TableViewColumn{
            id : imageColumn
            role : "icon"
            width : 25
            delegate : Component {
                Row {
                    width : 20
                    height : 18
                    spacing : 3
                    Image{
                        id : domicon
                        source : styleData.value ? ("../../images/" + styleData.value) : ""
                        width : 16
                        height : 16
                        anchors.verticalCenter: parent.verticalCenter
                    }

                }
            }
        }
        TableViewColumn{
            role : "attributename"
            title : qsTr("Name")
            width : 80
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }

        }
        TableViewColumn{
            role : "attributeDomainType"
            title : qsTr("Domain type")
            width : 90
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "min"
            title : qsTr("Minimum")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "max"
            title : qsTr("Maximum")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "average"
            title : qsTr("Average")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "median"
            title : qsTr("Median")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "sum"
            title : qsTr("Sum")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "distance"
            title : qsTr("Distance")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "variance"
            title : qsTr("Variance")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "skew"
            title : qsTr("Skew")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }
        TableViewColumn{
            role : "kurtosis"
            title : qsTr("Kurtosis")
            width : 70
            delegate : Component {
                Text {
                    text: styleData.value
                    verticalAlignment:Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }


    }
}


