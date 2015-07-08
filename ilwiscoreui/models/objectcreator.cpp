#include "resourcemodel.h"
#include "objectcreator.h"

using namespace Ilwis;

ObjectCreator::ObjectCreator(QObject *parent) : QObject(parent)
{

}

ObjectCreator::~ObjectCreator()
{

}

QStringList ObjectCreator::createableObjects() const
{
    QStringList nameiconpairs;
    nameiconpairs.append(TR("Numeric Domain") + "|" + ResourceModel::iconPath(itNUMERICDOMAIN));
    nameiconpairs.append(TR("Thematic Domain") + "|" + ResourceModel::iconPath(itITEMDOMAIN | itTHEMATICITEM));
    nameiconpairs.append(TR("Identifier Domain") + "|" + ResourceModel::iconPath(itITEMDOMAIN | itIDENTIFIERITEM));
    nameiconpairs.append(TR("Indexed Domain") + "|" + ResourceModel::iconPath(itITEMDOMAIN | itINDEXEDITEM));
    nameiconpairs.append(TR("Interval Domain") + "|" + ResourceModel::iconPath(itITEMDOMAIN | itNUMERICITEM));
    nameiconpairs.append(TR("Time Domain") + "|" + ResourceModel::iconPath(itTIME | itDOMAIN));
    nameiconpairs.append(TR("Time Interval Domain") + "|" + ResourceModel::iconPath(itTIMEITEM | itITEMDOMAIN));
    nameiconpairs.append(TR("Color Domain") + "|" + ResourceModel::iconPath(itCOLORDOMAIN));
    nameiconpairs.append(TR("Color Palette Domain") + "|" + ResourceModel::iconPath(itPALETTECOLOR | itITEMDOMAIN));
    nameiconpairs.append(TR("Corners Georeference") + "|" + ResourceModel::iconPath(itGEOREF));
    nameiconpairs.append(TR("Tiepoints Georeference") + "|" + ResourceModel::iconPath(itGEOREF | itLOCATION));
    nameiconpairs.append(TR("Projected Coordinate System") + "|" + ResourceModel::iconPath(itCONVENTIONALCOORDSYSTEM));
    nameiconpairs.append(TR("LatLon Coordinate System") + "|" + ResourceModel::iconPath(itCONVENTIONALCOORDSYSTEM|itLOCATION));
    nameiconpairs.append(TR("Bounds only Coordinate System") + "|" + ResourceModel::iconPath(itBOUNDSONLYCSY));
    nameiconpairs.append(TR("Raster Coverage") + "|" + ResourceModel::iconPath(itRASTER));
    nameiconpairs.append(TR("Feature Coverage") + "|" + ResourceModel::iconPath(itFEATURE));
    nameiconpairs.append(TR("Table") + "|" + ResourceModel::iconPath(itTABLE));
    nameiconpairs.append(TR("Representation") + "|" + ResourceModel::iconPath(itREPRESENTATION));

    return nameiconpairs;
}

