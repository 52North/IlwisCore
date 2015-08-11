#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "factory.h"
#include "abstractfactory.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "eigen3/Eigen/LU"
#include "mathhelper.h"
#include "planarctpgeoreference.h"
#include "undeterminedgeoreference.h"
#include "georefimplementationfactory.h"

using namespace Ilwis;

GeoRefImplementationFactory::GeoRefImplementationFactory() : AbstractFactory("georefimplementationfactory","ilwis","")
{
}

void GeoRefImplementationFactory::addCreator(const QString &name, CreateGeoRefImplementation func)
{
    auto iter = _implCreators.find(name);
    if ( iter == _implCreators.end()) {
        _implCreators[name] = func;
    }
}

GeoRefImplementation *GeoRefImplementationFactory::create(const QString& name)  {
    auto iter = _implCreators.find(name);
    if ( iter != _implCreators.end()) {
        return _implCreators[name]();
    }
    return 0;
}

bool GeoRefImplementationFactory::prepare()
{
    AbstractFactory::prepare();

    addCreator("simpel",SimpelGeoReference::create);
    addCreator("corners",CornersGeoReference::create);
    addCreator("undetermined",UndeterminedGeoReference::create);
    addCreator("tiepoints",PlanarCTPGeoReference::create);

    return true;
}


