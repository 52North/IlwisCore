#include "resourcemodel.h"
#include "ilwisobjectcreatormodel.h"
#include "objectcreator.h"

using namespace Ilwis;

ObjectCreator::ObjectCreator(QObject *parent) : QObject(parent)
{
    _creators.append(new IlwisObjectCreatorModel("Numeric Domain",itNUMERICDOMAIN,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Thematic Domain",itITEMDOMAIN | itTHEMATICITEM,"CreateThematicDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Identifier Domain",itITEMDOMAIN | itIDENTIFIERITEM,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Indexed Domain",itITEMDOMAIN | itINDEXEDITEM,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Interval Domain",itITEMDOMAIN | itNUMERICITEM,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Time Domain",itTIME | itDOMAIN,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Time Interval Domain",itTIMEITEM | itITEMDOMAIN,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Color Domain",itCOLORDOMAIN,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Color Palette Domain",itPALETTECOLOR | itITEMDOMAIN,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Corners Georeference",itGEOREF,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Tiepoints Georeference",itGEOREF | itLOCATION,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Projected Coordinate System",itCONVENTIONALCOORDSYSTEM,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("LatLon Coordinate System",itCONVENTIONALCOORDSYSTEM|itLOCATION,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Bounds only Coordinate System",itBOUNDSONLYCSY,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Raster Coverage",itRASTER,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Feature Coverage",itFEATURE,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Table",itTABLE,"CreateNumDom.qml", this));
    _creators.append(new IlwisObjectCreatorModel("Representation",itREPRESENTATION,"CreateNumDom.qml", this));
}

ObjectCreator::~ObjectCreator()
{

}

QStringList ObjectCreator::createableObjects() const
{
    QStringList nameiconpairs;
    for(auto *model : _creators)
        nameiconpairs.append(model->label() + "|" + ResourceModel::iconPath(model->type().toULongLong()));

    return nameiconpairs;
}

void ObjectCreator::setActiveCreator(qint32 index)
{
    if ( index >= 0 && index < _creators.size()){
        _activeCreators.clear();
        _activeCreators.append(_creators[index]);
        emit activeCreatorsChanged();
    }
}

QQmlListProperty<IlwisObjectCreatorModel> ObjectCreator::activeCreators()
{
    return  QQmlListProperty<IlwisObjectCreatorModel>(this, _activeCreators);
}


