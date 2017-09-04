#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "itemdomain.h"
#include "domainserializerv1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *DomainSerializerV1::create(QDataStream& stream)
{
    return new DomainSerializerV1(stream);
}

DomainSerializerV1::DomainSerializerV1(QDataStream &stream) : VersionedSerializer(stream)
{
}

bool DomainSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    Domain *dom = static_cast<Domain *>(obj);
    //quint64 valueType = dom->valueType();
//    _stream << valueType;

    if (!VersionedSerializer::store(obj, options))
        return false;

    if ( dom->ilwisType() == itITEMDOMAIN){
        ItemDomain<DomainItem> *itemdom= static_cast<ItemDomain<DomainItem> *>(dom) ;
        _stream << itemdom->theme();

    }
    if ( dom->ilwisType() != itTEXTDOMAIN)
        dom->range()->store(_stream);
    if ( dom->parent().isValid()){
        _stream << dom->parent()->resource().url().toString();
    }else
        _stream << QString(sUNDEF);

    _stream << dom->isStrict();

    return true;

}

bool DomainSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;


    Domain *dom = static_cast<Domain *>(obj);


    if ( dom->ilwisType() == itITEMDOMAIN){
        QString theme;
        _stream >> theme;
        ItemDomain<DomainItem> *itemdom= static_cast<ItemDomain<DomainItem> *>(dom) ;
        itemdom->setTheme(theme);

    }
    if ( dom->ilwisType() != itTEXTDOMAIN){
        Range * range = Range::create(dom->valueType());
        if ( !range)
            return false;
        range->load(_stream);
        dom->range(range);

    }
    QString parent;
    _stream >> parent;
    if ( parent != sUNDEF){
        IDomain parentdom;
        if ( parentdom.prepare(parent, {"mustexist", true})){
            dom->setParent(parentdom);
        }
    }
    bool strict;
    _stream >> strict;
    dom->setStrict(strict);


    return true;
}
