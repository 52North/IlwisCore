#include "kernel.h"
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "model.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "versioneddatastreamfactory.h"
#include "modellerfactory.h"
#include "modelserializerV1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *ModelSerializerV1::create(QDataStream& stream)
{
    return new ModelSerializerV1(stream);
}

ModelSerializerV1::ModelSerializerV1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool ModelSerializerV1::store(IlwisObject *obj,const IOOptions& options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Model *model = static_cast<Model *>(obj);

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    std::unique_ptr<DataInterface> wfstreamer(factory->create(Version::interfaceVersion, itMODEL,_stream));
    if ( !wfstreamer)
        return false;

    _stream << model->workflowCount();
    for(int i=0; i < model->workflowCount(); ++i){
        if(!wfstreamer->store(model->workflow(i).ptr(), options))
            return false;
    }
    _stream << model->analysisCount();
    for(int i=0; i < model->analysisCount(); ++i){
        model->analysisPattern(i)->store(_stream);
    }

    _stream << model->applicationCount();
    for(int i=0; i < model->applicationCount(); ++i){
        model->application(i)->store(_stream);
    }

    return true;
}

bool ModelSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    Model *model = static_cast<Model *>(obj);

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    std::unique_ptr<DataInterface> wfstreamer(factory->create(Version::interfaceVersion, itMODEL,_stream));
    if ( !wfstreamer)
        return false;

    qint32 count;

    _stream >> count;
    for(int i=0; i < count; ++i){

        quint64 type;
        QString version, url;
        _stream >> url;
        _stream >> type;
        _stream >> version;
        IWorkflow systemWf = makeSystemObject<IWorkflow>(url);
        IWorkflow wf (type);
        if(!wfstreamer->loadMetaData(wf.ptr(), options))
            return false;
        model->addWorklfow(systemWf.isValid() ? systemWf :wf );
    }
    _stream >> count;
    for(int i=0; i < count; ++i){
        QString type;
        _stream >> type;
        ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory","ilwis");
        if ( factory){
            AnalysisPattern *pattern = factory->createAnalysisPattern(type,sUNDEF,sUNDEF, options);
            if ( pattern){
                pattern->loadMetadata(_stream);
                model->addAnalysisPattern(pattern);
            }
        }
    }

    _stream >> count;
    for(int i=0; i < count; ++i){
        QString type;
        _stream >> type;
        ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory","ilwis");
        if ( factory){
            ApplicationModel *app = factory->createApplication(type);
            if ( app){
                app->loadMetadata(_stream);
                model->addApplication(app);
            }
        }
    }

    return true;
}

bool Ilwis::Stream::ModelSerializerV1::loadData(IlwisObject *data, const IOOptions &options)
{
    //TODO
    return false;
}

