/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.1
import QtGraphicalEffects 1.0
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

  Item {
      id: floatrect
      property alias text: toolTip.text
      z: 1000
      width: content.width + (2*toolTipShadow.radius)
      height: content.height + (2*toolTipShadow.radius)

      Rectangle {
          id: content
          anchors.centerIn: parent
          width: toolTip.contentWidth + 8
          height: toolTip.contentHeight + 4
          radius: 3
          color : "cornsilk"

          Text {
              id: toolTip
              wrapMode: Text.WordWrap
              x : 4
              anchors.verticalCenter: parent.verticalCenter
          }
      }
      opacity : 0
      enabled : false

  }

  DropShadow {
      id: toolTipShadow
      z: 1000
      anchors.fill: source
      cached: true
      horizontalOffset: 2
      verticalOffset: 2
      radius: 8.0
      samples: 16
      color: "#80000000"
      smooth: true
      source: floatrect
      opacity : floatrect.opacity
  }

  DropArea {
      anchors.fill : parent
      onDropped: {
          console.log (drag.source.ilwisobjectid);
          var resource = mastercatalog.id2Resource(drag.source.ilwisobjectid)
          addDataSource(resource.url, resource.name, resource.typeName)
      }
      LayersView {
          id: renderer
          anchors.fill: parent
          anchors.margins: 5

          MouseArea {
              id : mapArea
              anchors.fill: parent
              hoverEnabled: true

              onPressed: {
                  if ( manager.zoomInMode ){
                      if ( !manager.hasSelectionDrawer){
                          var position = {initialx: mouseX, initialy:mouseY}
                          manager.hasSelectionDrawer = true
                          renderer.addCommand("adddrawer(" + renderer.viewerId + ",selectiondrawer)")
                          renderer.setAttribute("SelectionDrawer", position)
                          renderer.update()
                      }

                  }
                  floatrect.enabled = true
                  floatrect.opacity = 1
                  floatrect.x = mouseX + 10
                  floatrect.y = mouseY  - 5

              }
              onPositionChanged: {
                  var mposition = mouseX + "|" + mouseY
                  renderer.currentCoordinate = mposition
                  if ( manager.hasSelectionDrawer){
                      var position = {currentx: mouseX, currenty:mouseY}
                      renderer.setAttribute("SelectionDrawer", position)
                      renderer.copyAttribute("SelectionDrawer","envelope");
                      renderer.update()
                  }
                  if ( floatrect.opacity > 0){
                      floatrect.x = mouseX + 10
                      floatrect.y = mouseY - 5
                  }
              }
              onReleased: {
                  if ( manager.zoomInMode && manager.hasSelectionDrawer){
                      var envelope = renderer.attributeOfDrawer("SelectionDrawer","envelope");
                      renderer.addCommand("removedrawer(" + renderer.viewerId + ",selectiondrawer,post)");
                      if ( envelope !== ""){
                          renderer.addCommand("setviewextent("+ renderer.viewerId + "," + envelope + ")");
                      }
                      manager.hasSelectionDrawer = false
                      renderer.update()
                  }
                  floatrect.enabled = false
                  floatrect.opacity = 0
                  floatrect.x = 0
                  floatrect.y = 0
              }
          }
      }
   }
}
