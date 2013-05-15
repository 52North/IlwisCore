#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include "angle.h"
#include "point.h"
#include "box.h"
#include "ilwisobject.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "valuedefiner.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "boost/numeric/ublas/matrix.hpp"
#include "boostext.h"
#include "coverage.h"
#include "georeference.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "grid.h"
#include "gridcoverage.h"
#include "pixeliterator.h"
#include "gridcoveragegenerator.h"

using namespace Ilwis;

GridCoverageGenerator::GridCoverageGenerator()
{
}

bool GridCoverageGenerator::setSize(const Box2D<qint32>& box, const Box2D<double> bounds) {
    Size szOut(box.width(),box.height());

    if ( szOut != size()) {
        CornersGeoReference *grf = new CornersGeoReference();
        grf->setName("subset_" + name());
        grf->setCoordinateSystem(coordinateSystem());
        grf->setEnvelope(bounds);
        IGeoReference georef;
        georef.set(static_cast<GeoReference *>(grf));
        georef->compute(szOut.toQSize());
        setGeoreference(georef);
    }
    return true;
}

bool GridCoverageGenerator::setDomain(const QString &nme)
{
    IDomain dom;
    if ( !dom.prepare(nme)) {
        return ERROR2(ERR_NO_INITIALIZED_2, "domain", name());
    }
    GridCoverage::setDomain(dom);
}

bool GridCoverageGenerator::generateFrom(const IGridCoverage& inputGC, DFunctionD func)
{
    int n = 1; //QThread::idealThreadCount();
    Size size = inputGC->size();
    double step = size.ysize(); // / n;
    QList<Slice> slices;
    for(int line =0; line < size.ysize() ; line += step ) {
        Slice slice;
        slice._calc = func;
        slice._inputgc = inputGC;
        slice._outputgc = this;
        slice._minLine = line;
        slice._maxline = line + step;
        slices.push_back(slice);
    }
 //   QFuture<void> newcoverage = QtConcurrent::map(slices,&Ilwis::Slice::calculateSlice);
  //  newcoverage.waitForFinished();
    slices[0].calculateSlice();
    foreach(Slice s, slices)
        if(!s._status)
            return false;

    return true;
}



//-------------------------------------------------

void Slice::calculateSlice()
{
    if (!_inputgc.isValid()) {
        kernel()->issues()->log(TR(ERR_NO_INITIALIZED_1).arg("Input GridCoverage"));

        return ;
    }
    if (!((IGridCoverage *)_outputgc)->isValid()) {
        kernel()->issues()->log(TR(ERR_NO_INITIALIZED_1).arg("Output GridCoverage"));
        return ;
    }

    PixelIterator iterIn(_inputgc, Box2D<>(_inputgc->size().toQSize()));
    PixelIterator iterOut(_outputgc, Box2D<>(_outputgc->size().toQSize()));

    double v_in;
    for_each(iterOut, iterOut.end(), [&](double& v){
        if ( (v_in = *iterIn) != rUNDEF)
            v = _calc(v_in);
        ++iterIn;
    });




//    unsigned long columns = _outputgc->gridSize().x();
//    for(unsigned long y = _minLine; y < _maxline; ++y) {
//        for(unsigned long x = 0; x < columns; ++x) {
//            Pixel location(y,x);
//            double v = _inputgc->value(location);
//            if ( v != rUNDEF)
//                v = _calc(v) ;
//            bool ok = _outputgc->setValue(location,v);
//            if (!ok)
//                return ;
//        }
//    }
    _status = true ;
}
