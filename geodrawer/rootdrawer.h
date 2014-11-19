#ifndef ROOTDRAWER_H
#define ROOTDRAWER_H

#include <memory.h>
#include <QMatrix4x4>
#include <QObject>
#include "identity.h"
#include "complexdrawer.h"

namespace Ilwis {
namespace Geodrawer{

class RootDrawer : public ComplexDrawer
{
    Q_OBJECT
public:
    explicit RootDrawer(QObject *parent);

    Envelope viewEnvelope() const;
    void coordBoundsView(const Envelope& viewRect, bool overrule);
    void rasterSize(const Size<> size);
    Size<> rasterSize() const;
    const QMatrix4x4 *mvpMatrix() const;

    void viewPoint(const Coordinate &viewCenter, bool setEyePoint=false);
    void cleanUp();
signals:

public slots:

private:
    QMatrix4x4 _view,_projection, _model, _mvp;
    Envelope _zoomRect; // extent of the portion of the map now visible in the visualization
    Envelope _viewRect; // extent of the whole area that is covered by the visualization. this might be bigger than the map due to the aspect ratio of the map
    Envelope _coverageRect; // extent of coverage without any additional area
    Size<> _rasterSize; // size of the area in the viewRect in pixels
    double _aspectRatioCoverage; //ration between width/height of the coverage. determines how sides of a map will size in reaction to size changes
    Coordinate _viewPoint;
    Coordinate _eyePoint;
};

typedef std::unique_ptr<RootDrawer> UPRootDrawer;
}
}

#endif // ROOTDRAWER_H
