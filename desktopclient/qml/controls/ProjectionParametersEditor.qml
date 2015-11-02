import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import ResourceModel 1.0
import IlwisObjectModel 1.0
import "../Global.js" as Global
import "../controls" as Controls

ListView {
    function projectionParameters(){
        var values = []
        if ( model === undefined)
            return values
        for(var i=0; i < items.length; ++i){
             values.push(items[i].parent.bb)
        }
        return values
    }

    Component {
        id : editors
        Item {


            width : parent.width
            height : 21

            Row {
                width : parent.width
                height : parent.height + 3
                Text {
                    x : 3
                    text: name
                    width : 120
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 10
                    elide: Text.ElideMiddle
                }
                Item {
                    width : 120
                    height : 18
                    property int bb : fieldvalue()
                    function fieldvalue(){

                        if ( vvalue.width != 0)
                            return vvalue.text
                        if ( llvalue.width != 0 )
                            return llvalue.metric
                        if ( bvalue.width != 0)
                            return bvalue.checked ? 1 : 0
                        return -100000
                    }
                    Controls.ValidatedTextField{
                        id : vvalue
                        width : (valueType === "real64" || valueType=== "signedbyte") ? parent.width : 0
                        opacity : (valueType === "real64" || valueType=== "signedbyte") ? 1 : 0
                        enabled : (valueType === "real64" || valueType=== "signedbyte") ? true :false
                        height : 18
                        text : value
                    }
                    Controls.LatLonCoordinate{
                        id : llvalue
                        width : valueType === "latlon" ? parent.width + 20 : 0
                        opacity : valueType === "latlon" ? 1 : 0
                        enabled : valueType === "latlon" ? true :false
                        height : 18
                        Component.onCompleted: {
                            setCoordinate(value)
                        }
                   }
                   CheckBox{
                        id : bvalue
                        width : valueType === "boolean" ? parent.width : 0
                        opacity : valueType === "boolean" ? 1 : 0
                        enabled : valueType === "boolean" ? true :false
                        checked : value == "true" ? true : false
                    }
                }
                Component.onCompleted: {
                    if ( vvalue.width != 0)
                        items.push(vvalue)
                    else if ( llvalue.width != 0 )
                        items.push(llvalue)
                    else if ( bvalue.width != 0)
                        items.push(bvalue)
                    else
                        console.debug(valueType)
                }
            }
        }
    }

    property var items : []
    delegate: editors
    clip : true


}

