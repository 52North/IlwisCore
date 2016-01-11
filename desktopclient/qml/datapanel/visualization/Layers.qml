import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls
import LayersView 1.0

Item {

  signal zoomEnded(string envelope)

  function addDataSource(filter, sourceName, sourceType){
      if ( filter.indexOf("=") !== -1){
          filter = "\"" + filter + "\""
      }
      renderer.addCommand("adddrawer(" + renderer.viewerId + ","+ sourceName + "," + filter + "," + sourceType + ")")
      renderer.update()
  }

  function entireMap() {
      renderer.addCommand("setviewextent("+ renderer.viewerId + ",entiremap)");
      renderer.update()
  }

  function newExtent(ext){
      renderer.addCommand("setviewextent("+ renderer.viewerId + "," + ext + ")");
      renderer.update()
  }

  function transfer(datapanel){
      var layers = datapanel.manager.layers;
      for(var i =1; i < layers.length; i++){  // start at 1 because the first layer is always the global layer, is there by default so we skip it
          var expr = "adddrawer(" + renderer.viewerId + ","+ layers[i].name + ",\"itemid=" + layers[i].id + "\"," + layers[i].typeName + ")"
          renderer.addCommand(expr)
      }
      renderer.update()
  }

  function setManager(layermanager){
      renderer.setManager(layermanager)
  }

  function drawer(){
      return renderer
  }



  DropArea {
      anchors.fill : parent
      onDropped: {
          var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid)
          addDataSource(resource.url, resource.name, resource.typeName)
      }

      Connections {
          target: mouseActions
          onZoomEnded :{
              zoomEnded(envelope)
          }
      }

      Item {
          anchors.fill: parent
          LayersView {
              id: renderer
              anchors.top: parent.top
              height : parent.height - hscroller.height
              width : parent.width - vscroller.width
              anchors.margins: 1
              objectName : "layers_" + uicontext.uniqueName()

              LayerExtentMouseActions{
                  id : mouseActions
                  layerManager: manager
                  drawer : renderer
                  linkedDrawer: renderer
                  showInfo: true
              }
              Component.onCompleted: {
                  renderer.associate(hscroller.objectName,"synchronizeEnded")
                  renderer.associate(vscroller.objectName,"synchronizeEnded")
              }
          }
          Controls.HScrollBar{
              anchors.top : renderer.bottom
              id :hscroller
              objectName: uicontext.uniqueName()

              function updateItem(){
                  var envView = renderer.viewEnvelope
                  maxSize = envView.maxx - envView.minx
                  var envZoom = renderer.zoomEnvelope
                  oldPosition = -10000
                  currentSize = envZoom.maxx - envZoom.minx
                  if ( currentPosition === -1)
                    currentPosition = 0
                  else{
                      currentPosition = envZoom.minx - envView.minx

                  }

                  return 0
              }

              onScrolled: {
                  var envZoom = renderer.zoomEnvelope
                  var envView = renderer.viewEnvelope
                  var newPos1 = envView.minx + position
                  var newPos2 = newPos1 + envZoom.maxx - envZoom.minx
                  envZoom.minx = newPos1
                  envZoom.maxx = newPos2
                  renderer.zoomEnvelope = envZoom

              }
          }
          Controls.VScrollBar{
              anchors.left : renderer.right
              id :vscroller
              objectName: uicontext.uniqueName()

              function updateItem(){
                  var envView = renderer.viewEnvelope
                  maxSize = envView.maxy - envView.miny
                  var envZoom = renderer.zoomEnvelope
                  currentSize = envZoom.maxy - envZoom.miny
                  oldPosition = -10000
                  if ( currentPosition === -1)
                    currentPosition = 0
                  else{
                      currentPosition = envView.maxy - envZoom.maxy
                  }

                  return 0
              }

              onScrolled: {
                  var envZoom = renderer.zoomEnvelope
                  var envView = renderer.viewEnvelope
                  var newPos1 = envView.maxy - position
                  var newPos2 = newPos1 - (envZoom.maxy - envZoom.miny)
                  envZoom.miny = newPos2
                  envZoom.maxy = newPos1
                  renderer.zoomEnvelope = envZoom

              }
          }
      }
  }
}
