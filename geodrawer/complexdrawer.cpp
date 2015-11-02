#include "kernel.h"
#include "errorobject.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

ComplexDrawer::ComplexDrawer(const QString &name,
                             DrawerInterface* parentDrawer,
                             RootDrawer *rootdrawer,
                             const IOOptions &options) : BaseDrawer(name, parentDrawer, rootdrawer, options)
{

}

bool ComplexDrawer::draw(const IOOptions &options)
{
    if (!isActive() || !isValid())
        return false;

    bool ok = drawSideDrawers( _preDrawers, options)    ;

    for(const auto& drawer : _mainDrawers){
        if ( drawer){
            ok &= drawer->draw( options);
        }
    }

    ok &= drawSideDrawers(_postDrawers, options);


    return ok;
}

bool ComplexDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    BaseDrawer::prepare(prepType, options);

    for( auto& drawer : _preDrawers)    {
        if (!drawer.second->prepare(prepType, options))
            return false;
    }
    for( auto& drawer : _mainDrawers){
        if (!drawer->prepare(prepType, options))
            return false;
    }
    for( auto& drawer : _postDrawers)    {
        if (!drawer.second->prepare(prepType, options))
            return false;
    }
    return true;

}

void ComplexDrawer::unprepare(DrawerInterface::PreparationType prepType)
{
    BaseDrawer::unprepare(prepType);

    for( auto& drawer : _preDrawers)    {
        drawer.second->unprepare(prepType);
    }
    for( auto& drawer : _mainDrawers){
        drawer->unprepare(prepType);
    }
    for( auto& drawer : _postDrawers)    {
        drawer.second->unprepare(prepType);
    }
}

bool ComplexDrawer::prepareChildDrawers(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    for(auto& drawer : _mainDrawers){
        if ( drawer && drawer->isValid())
            drawer->prepare(prepType, options);
    }
    return true;
}

quint32 ComplexDrawer::drawerCount(DrawerInterface::DrawerType tpe) const
{
    switch(tpe){
    case dtPRE:
        return _postDrawers.size();
    case dtMAIN:
        return _mainDrawers.size();
    case dtPOST:
        return _postDrawers.size();
    default:
        return _postDrawers.size() + _preDrawers.size() + _mainDrawers.size();;
    }
    return iUNDEF;
}

const UPDrawer &ComplexDrawer::drawer(quint32 order, DrawerInterface::DrawerType drawerType) const
{
    if ( drawerType == dtPOST || drawerType == dtPRE){
        const DrawerMap& drawers = drawerType == dtPRE ? _preDrawers : _postDrawers;
        const auto& current = drawers.find(order);
        if ( current == drawers.end())
            throw VisualizationError(TR("Invalid drawer number used while drawing"));

        return current->second;
    } else if ( drawerType == dtMAIN){
        if ( order < _mainDrawers.size()){
            return _mainDrawers[order];
        }
    }
    throw VisualizationError(TR(QString("Drawer number %1 is not valid").arg(order)));
}

UPDrawer &ComplexDrawer::drawer(quint32 index, DrawerInterface::DrawerType drawerType)
{
    if ( drawerType == dtPOST || drawerType == dtPRE){
        DrawerMap& drawers = drawerType == dtPRE ? _preDrawers : _postDrawers;
        auto current = drawers.find(index);
        if ( current == drawers.end())
            throw VisualizationError(TR("Invalid drawer number used while drawing"));

        return current->second;
    } else if ( drawerType == dtMAIN){
        if ( index < _mainDrawers.size()){
            return _mainDrawers[index];
        }
    }
    throw VisualizationError(TR(QString("Drawer number %1 is not valid").arg(index)));
}

UPDrawer &ComplexDrawer::drawer(const QString &namecode, DrawerInterface::DrawerType drawerType)
{
    if ( drawerType == dtPOST || drawerType == dtPRE){
        DrawerMap& drawers = drawerType == dtPRE ? _preDrawers : _postDrawers;
        for(auto& drawer : drawers){
            if ( drawer.second->name() == namecode || namecode == drawer.second->code())
                return drawer.second;
        }
        throw VisualizationError(TR("Invalid drawer number used while drawing"));

    } else if ( drawerType == dtMAIN){
        for(auto& drawer : _mainDrawers){
            if ( drawer->name() == namecode  ||  namecode == drawer->code())
                return drawer;
        }
    }
    throw VisualizationError(TR(QString("Drawer number %1 is not valid").arg(namecode)));
}

const UPDrawer &ComplexDrawer::drawer(const QString &namecode, DrawerInterface::DrawerType drawerType) const
{
    if ( drawerType == dtPOST || drawerType == dtPRE){
        const DrawerMap& drawers = drawerType == dtPRE ? _preDrawers : _postDrawers;
        for(auto& drawer : drawers){
            if ( drawer.second->name() == namecode || namecode == drawer.second->code())
                return drawer.second;
        }
        throw VisualizationError(TR("Invalid drawer number used while drawing"));

    } else if ( drawerType == dtMAIN){
        for(auto& drawer : _mainDrawers){
            if ( drawer->name() == namecode  ||  namecode == drawer->code())
                return drawer;
        }
    }
    throw VisualizationError(TR(QString("Drawer number %1 is not valid").arg(namecode)));
}

void ComplexDrawer::addDrawer(DrawerInterface *drawer, DrawerInterface::DrawerType drawerType, quint32 order,const QString &nme)
{
    if ( !drawer)
        return;

    QString drawername = nme;
    if ( drawerType == dtPOST || drawerType == dtPRE){
        if ( order == iUNDEF)
            order = drawer->defaultOrder();
        DrawerMap& drawers = drawerType == dtPRE ? _preDrawers : _postDrawers;
        drawers[order].reset(drawer);
        if ( drawername == sUNDEF){
            drawername = (drawerType == dtPOST ? "post_drawer_" : "pre_drawer_") + QString::number(drawer->id()) + "_" + QString::number(order);

        }
    }else if ( drawerType == dtMAIN){
        UPDrawer drawerp(drawer);
        _mainDrawers.push_back(std::move(drawerp));
        if ( drawername == sUNDEF){
            drawername = "drawer_layer_" + QString::number(drawer->id());
        }

    }
    drawer->name(drawername);
    if ( drawerType == DrawerInterface::dtMAIN)
        drawer->code(drawername);
}

void ComplexDrawer::setDrawer(quint32 order, DrawerInterface *drawer, DrawerInterface::DrawerType drawerType)
{
    if ( drawerType == dtPOST || drawerType == dtPRE){
        DrawerMap& drawers = drawerType == dtPRE ? _preDrawers : _postDrawers;
        const auto& current = drawers.find(order);
        if ( current == drawers.end()){
            return;
        }
        drawers[order].reset(drawer);
    } else if ( drawerType == dtMAIN){
        if ( order < _mainDrawers.size()) {
            _mainDrawers[order].reset(drawer);
        }
    }
}

void ComplexDrawer::removeDrawer(quint32 order, DrawerInterface::DrawerType drawerType)
{
    if ( drawerType == dtPOST || drawerType == dtPRE){
        DrawerMap& drawers = drawerType == dtPRE ? _preDrawers : _postDrawers;
        auto current = drawers.find(order);
        if ( current != drawers.end()){
            (*current).second->cleanUp();
            drawers.erase(current)    ;

        }
    } else if ( drawerType == dtMAIN){
        if ( order < _mainDrawers.size()) {
           _mainDrawers[order]->cleanUp();
           _mainDrawers.erase(_mainDrawers.begin() + order);
        }
    }
}

void ComplexDrawer::removeDrawer(const QString &idcode, bool ascode)
{
    std::vector<quint32> selectedDrawers;
    for( auto& drawer : _preDrawers){
        if ( ascode){
            if (drawer.second->code() == idcode){
                selectedDrawers.push_back(drawer.first);
            }
        } else {
            if (drawer.second->name() == idcode){
                selectedDrawers.push_back(drawer.first);
            }
        }
    }
    for(auto order : selectedDrawers)
        removeDrawer(order, DrawerInterface::dtPRE);
    selectedDrawers.clear();
    int count = 0;
    for( auto& drawer : _mainDrawers){
        if ( ascode){
            if (drawer->code() == idcode){
                selectedDrawers.push_back(count);
            }
        } else {
            if (drawer->name() == idcode){
                selectedDrawers.push_back(count);
            }
        }
        ++count;
    }
    for(auto order : selectedDrawers)
        removeDrawer(order, DrawerInterface::dtMAIN);
    selectedDrawers.clear();

    for( auto& drawer : _postDrawers)    {
        if ( ascode){
            if (drawer.second->code() == idcode){
                selectedDrawers.push_back(drawer.first);
            }
        } else {
            if (drawer.second->name() == idcode){
                selectedDrawers.push_back(drawer.first);
            }
        }
    }
    for(auto order : selectedDrawers)
        removeDrawer(order, DrawerInterface::dtPOST);
}

bool ComplexDrawer::drawerAttribute(const QString &drawercode, const QString &key, const QVariant &value)
{
    if ( drawercode == code() || name() == drawercode){
        setAttribute(key, value);
        return true;
    }

    for( auto& drawer : _preDrawers){
        if (drawer.second->drawerAttribute(drawercode, key, value))
            return true;
    }
    for( auto& drawer : _mainDrawers){
        if (drawer->drawerAttribute(drawercode, key, value))
            return true;
    }
    for( auto& drawer : _postDrawers)    {
        if (drawer.second->drawerAttribute(drawercode, key, value))
            return true;
    }

    return false;
}

bool ComplexDrawer::isSimple() const
{
    return false;
}

bool ComplexDrawer::hasDrawer(const QString &namecode, DrawerInterface::DrawerType drawerType) const
{
    if ( drawerType == dtPOST || drawerType == dtPRE || drawerType == dtALL){
        const DrawerMap& drawers = drawerType == dtPRE ? _preDrawers : _postDrawers;
        for(auto& drawer : drawers){
            if ( drawer.second->name() == namecode || namecode == drawer.second->code())
                return true;
        }

    } else if ( drawerType == dtMAIN || drawerType == dtALL){
        for(auto& drawer : _mainDrawers){
            if ( drawer->name() == namecode  ||  namecode == drawer->code())
                return true;
        }
    }
    return false;
}

void ComplexDrawer::cleanUp()
{
    BaseDrawer::cleanUp();

    for( auto& drawer : _preDrawers)    {
        drawer.second->cleanUp();
    }
    for( auto& drawer : _mainDrawers){
        drawer->cleanUp();
    }
    for( auto& drawer : _postDrawers)    {
        drawer.second->cleanUp();
    }
    _preDrawers = DrawerMap();
    _mainDrawers = DrawerList();
    _postDrawers= DrawerMap();
    unprepare(ptALL);

}

bool ComplexDrawer::drawSideDrawers(const DrawerMap& drawers, const IOOptions &options) const
{
    if (!isActive())
        return false;

    if ( drawers.size() > 0) {
        for(const auto& current : drawers) {
            const auto& drw = current.second;
            if ( drw)
                drw->draw(options);
        }
    }
    return true;
}

std::vector<QVariant> ComplexDrawer::attributes(const QString &attrNames) const
{
    std::vector<QVariant> results = BaseDrawer::attributes(attrNames);
    QStringList parts = attrNames.split("|");
    for(QString& part : parts)
        part.toLower();

    for(const QString& part : parts){
        results.push_back(attribute(part));
    }
    return results;
}

QVariant ComplexDrawer::attribute(const QString &attrNme) const
{
    QString attrName = attrNme.toLower();
    QVariant var = BaseDrawer::attribute(attrName);
    if ( var.isValid())
        return var;

    if ( attrName == "maindrawercount")
        return drawerCount(DrawerInterface::dtMAIN);
    if ( attrName == "predrawercount")
        return drawerCount(DrawerInterface::dtPRE);
    if ( attrName == "postdrawercount")
        return drawerCount(DrawerInterface::dtPOST);


    return QVariant();
}

QVariant ComplexDrawer::execute(const QString &operationName, const QVariantMap &parameters)
{
    if ( operationName.toLower() == "layerremove"){
        if ( parameters.size() == 1) {
            QVariant var = parameters.begin().value();
            bool ok;
            quint32 layer = var.toUInt(&ok);
            if ( ok){
                removeDrawer(layer);
                return QVariant(true);
            }
        }
    }
    if ( operationName.toLower() == "layerdown"){
        if ( parameters.size() == 1) {
            QVariant var = parameters.begin().value();
            bool ok;
            quint32 layer = var.toUInt(&ok);
            if ( ok && layer <_mainDrawers.size() && _mainDrawers.size() >= 2){
                _mainDrawers[layer].swap(_mainDrawers[layer - 1]);
                return QVariant(true);
            }
        }
    }

    if ( operationName.toLower() == "layerup"){
        if ( parameters.size() == 1) {
            QVariant var = parameters.begin().value();
            bool ok;
            quint32 layer = var.toUInt(&ok);
            if ( ok && layer <_mainDrawers.size() - 1 && _mainDrawers.size() >= 2){
                _mainDrawers[layer].swap(_mainDrawers[layer + 1]);
                return QVariant(true);
            }
        }
    }
    if ( operationName == "code2index" ){
        if ( parameters.size() == 1){
            int index = 0;
            QString cod = parameters["code"].toString();
            for( auto& drawer : _mainDrawers){
                if ( drawer->code() == cod){
                    return QVariant(index);
                }
                ++index;
            }
        }
    }
    if ( operationName == "index2code" ){
        if ( parameters.size() == 1){
            bool ok;
            int index = parameters["index"].toUInt(&ok);
            if ( index < _mainDrawers.size()){
                return QVariant ( _mainDrawers[index]->code());
            }
        }
    }
    return QVariant();
}

QVariant ComplexDrawer::attributeOfDrawer(const QString &drawercode, const QString &attrName) const
{
    if ( name() == drawercode ||  code() == drawercode)
        return attribute(attrName);
    if ( drawercode ==  "rootdrawer"){ // special case
        return rootDrawer()->attribute(attrName);
    }

    QVariant var;
    for( auto& drawer : _preDrawers){
        var = drawer.second->attributeOfDrawer(drawercode, attrName);
        if ( var.isValid())
            return var;

    }
    for( auto& drawer : _mainDrawers){
        var = drawer->attributeOfDrawer(drawercode, attrName);
        if ( var.isValid())
            return var;
    }
    for( auto& drawer : _postDrawers)    {
        var = drawer.second->attributeOfDrawer(drawercode, attrName);
        if ( var.isValid())
            return var;
    }
    return var;
}
