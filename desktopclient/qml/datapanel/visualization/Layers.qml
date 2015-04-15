import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import "../../controls" as Controls
import LayersView 1.0

Item {


  function addDataSource(sourceUrl, sourceName, sourceType){
      renderer.addCommand("adddrawer(" + renderer.viewerId + ","+ sourceName + "," + sourceUrl + "," + sourceType + ")")
      renderer.update()
      //layertools.model = manager.layers
  }

  function entireMap() {
      var envelope = renderer.attributeOfDrawer("rootdrawer","coverageenvelope");
      if ( envelope !== ""){
          renderer.addCommand("setviewextent("+ renderer.viewerId + "," + envelope + ")");
      }
      renderer.update()
  }

  function transferLayers(layermanager){
      var layers = layermanager.layers;
      for(var i =1; i < layers.length; i++){  // start at 1 because the first layer is always the global layer, is there by default so we skip it
          renderer.addCommand("adddrawer(" + renderer.viewerId + ","+ layers[i].name + "," + layers[i].url + "," + layers[i].typeName + ")")
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

      LayersView {
          id: renderer
          anchors.fill: parent
          anchors.margins: 5



          LayerExtentMouseActions{
              layerManager: manager
          }
      }


  }
}
