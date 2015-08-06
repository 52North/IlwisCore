#include "kernel.h"
#include "visualattributemodel.h"
#include "globallayermodel.h"
#include "gridpropertyeditor.h"

REGISTER_PROPERTYEDITOR("primarygridpropertyeditor",PrimaryGridEditor)
REGISTER_PROPERTYEDITOR("secondarygridpropertyeditor",SecondaryGridEditor)

using namespace Ilwis;
using namespace Geodrawer;

SubGridPropertyEditor::SubGridPropertyEditor(const QString& editname, const QString& label,const QUrl& qml, QObject *parent) :
    VisualAttributeEditor(editname,label,qml, parent)
{

}

bool SubGridPropertyEditor::canUse(const IIlwisObject &obj, const QString &name) const{
    bool ok = name == "gridpropertyeditor";
    return ok;
}

UPDrawer &SubGridPropertyEditor::subgriddrawer(const QString& gridname)
{
    return attribute()->layer()->drawer()->drawer("GridDrawer", DrawerInterface::dtPOST)->drawer(gridname,DrawerInterface::dtMAIN);
}

const UPDrawer &SubGridPropertyEditor::subgriddrawer(const QString& gridname) const
{
    return attribute()->layer()->drawer()->drawer("GridDrawer", DrawerInterface::dtPOST)->drawer(gridname,DrawerInterface::dtMAIN);
}

double SubGridPropertyEditor::opacity() const
{
    const GlobalLayerModel *globalLayer = static_cast<const GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer)
        return globalLayer->drawer()->attributeOfDrawer(gridDrawerName(), "gridlineopacity").toDouble();
    return rUNDEF;
}

void SubGridPropertyEditor::opacity(double v)
{

    GlobalLayerModel *globalLayer = static_cast<GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer && v >= 0 && v <= 1){
        Ilwis::Geodrawer::UPDrawer& griddrawer =  subgriddrawer(gridDrawerName());
        griddrawer->setAttribute("gridlineopacity", v);
        griddrawer->unprepare(DrawerInterface::ptRENDER);
        griddrawer->prepare(DrawerInterface::ptRENDER, IOOptions());
        griddrawer->redraw();
    }

}

QColor SubGridPropertyEditor::lineColor() const
{

    const GlobalLayerModel *globalLayer = static_cast<const GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer)
        return globalLayer->drawer()->attributeOfDrawer(gridDrawerName(), "gridlinecolor").value<QColor>();

    return QColor();

}

void SubGridPropertyEditor::lineColor(const QColor &clr)
{
   GlobalLayerModel *globalLayer = static_cast<GlobalLayerModel *>(attribute()->layer());
   if ( globalLayer){
        Ilwis::Geodrawer::UPDrawer& griddrawer =  subgriddrawer(gridDrawerName());
        griddrawer->setAttribute("gridlinecolor", qVariantFromValue(clr));
        griddrawer->unprepare(DrawerInterface::ptRENDER);
        griddrawer->prepare(DrawerInterface::ptRENDER, IOOptions());
        griddrawer->redraw();
   }
}

void SubGridPropertyEditor::isActive(bool yesno){
    GlobalLayerModel *globalLayer = static_cast<GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer ){
        Ilwis::Geodrawer::UPDrawer& griddrawer =  subgriddrawer(gridDrawerName());
        griddrawer->setAttribute("active", yesno);
        griddrawer->redraw();
    }
}

//-----------------------------------------

PrimaryGridEditor::PrimaryGridEditor(QObject *parent) : SubGridPropertyEditor("primarygridpropertyeditor",TR("Primary Grid"),QUrl("GridPropertyEditor.qml"), parent)
{

}

double PrimaryGridEditor::distance() const
{
    const GlobalLayerModel *globalLayer = static_cast<const GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer){
        const UPDrawer& griddrawer = subgriddrawer(gridDrawerName());
        return griddrawer->attribute("gridcelldistance").toDouble() ;
    }
    return rUNDEF;
}

void PrimaryGridEditor::distance(double v)
{
    GlobalLayerModel *globalLayer = static_cast<GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer && v > 0){
        Ilwis::Geodrawer::UPDrawer& griddrawer = subgriddrawer(gridDrawerName());
        griddrawer->setAttribute("gridcelldistance", v);
        griddrawer->unprepare(DrawerInterface::ptGEOMETRY);
        griddrawer->prepare(DrawerInterface::ptGEOMETRY, IOOptions());
        griddrawer->redraw();
    }
}

bool PrimaryGridEditor::isActive() const{
    GlobalLayerModel *globalLayer = static_cast<GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer){
         const UPDrawer& griddrawer =  subgriddrawer(gridDrawerName());
         return griddrawer->isActive();
    }
    return false;
}

void PrimaryGridEditor::isActive(bool yesno)
{
    GlobalLayerModel *globalLayer = static_cast<GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer ){
        Ilwis::Geodrawer::UPDrawer& griddrawer =  subgriddrawer("PrimaryGridDrawer");
        griddrawer->setAttribute("active", yesno);
        griddrawer->redraw();
    }
}

QString PrimaryGridEditor::gridDrawerName() const
{
    return "PrimaryGridDrawer";
}

void PrimaryGridEditor::prepare(VisualAttributeModel *vattrib, const Ilwis::IIlwisObject& bj, const Ilwis::ColumnDefinition &datadef){
    SubGridPropertyEditor::prepare(vattrib,bj,datadef);
    //the create name is a generalized name, now we can set it to the specific name
    name("primarygridpropertyeditor");
    displayName(TR("Primary Grid"));

}

VisualAttributeEditor *PrimaryGridEditor::create(){
    return new PrimaryGridEditor();
}

//-----------------------------------------------------
SecondaryGridEditor::SecondaryGridEditor(QObject *parent) :
    SubGridPropertyEditor("secondarygridpropertyeditor",TR("Secondary Grid"),QUrl("GridPropertyEditor.qml"), parent)
{

}

int SecondaryGridEditor::noOfCells() const
{
    const GlobalLayerModel *globalLayer = static_cast<const GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer){
        const UPDrawer& griddrawer = subgriddrawer(gridDrawerName());
        return griddrawer->attribute("cellcount").toInt() ;
    }
    return iUNDEF;
}

void SecondaryGridEditor::noOfCells(int v)
{
    GlobalLayerModel *globalLayer = static_cast<GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer && v > 0){
        Ilwis::Geodrawer::UPDrawer& griddrawer = subgriddrawer(gridDrawerName());
        griddrawer->setAttribute("cellcount", v);
        griddrawer->unprepare(DrawerInterface::ptGEOMETRY);
        griddrawer->prepare(DrawerInterface::ptGEOMETRY, IOOptions());
        griddrawer->redraw();
    }
}

bool SecondaryGridEditor::isActive() const{
    GlobalLayerModel *globalLayer = static_cast<GlobalLayerModel *>(attribute()->layer());
    if ( globalLayer){
        bool canBeActive = true;
        const UPDrawer& pgriddrawer =  subgriddrawer("PrimaryGridDrawer");
         canBeActive = pgriddrawer->isActive();
         const UPDrawer& griddrawer =  subgriddrawer(gridDrawerName());
         return griddrawer->isActive() && canBeActive;
    }
    return false;
}

QString SecondaryGridEditor::gridDrawerName() const
{
    return "SecondaryGridDrawer";
}

VisualAttributeEditor *SecondaryGridEditor::create()
{
    return new SecondaryGridEditor();
}

void SecondaryGridEditor::prepare(VisualAttributeModel *vattrib, const Ilwis::IIlwisObject& bj, const Ilwis::ColumnDefinition &datadef){
    SubGridPropertyEditor::prepare(vattrib,bj,datadef);
    //the create name is a generalized name, now we can set it to the specific name
    name("secondarygridpropertyeditor");
    displayName(TR("Secondary Grid"));

}

void SecondaryGridEditor::isActive(bool yesno)
{
    SubGridPropertyEditor::isActive(yesno);
}

