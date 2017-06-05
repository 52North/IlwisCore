#include "kernel.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "pythonscriptconnector.h"
#include "pythonobjectfactory.h"

using namespace Ilwis;
using namespace PythonScript;

PythonObjectFactory::PythonObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","python","")
{

}

IlwisObject *Ilwis::PythonScript::PythonObjectFactory::create(const Ilwis::Resource &resource, const IOOptions &options) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    PythonScriptConnector *connector = factory->createFromResource<PythonScriptConnector>(resource, "python", options);

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
   IlwisObject *object = createObject(connector, options);
   if ( object)
       return object;

   delete connector;
   return 0;
}

bool PythonObjectFactory::canUse(const Resource &resource) const
{
    if( resource.url().scheme() == "ilwis")  {
        return false;
    }
    QString filename = resource.url(true).toLocalFile();
    if ( filename == "")
        return false;
    QString ext = QFileInfo(filename).suffix().toLower();
    if ( ext != "py")
        return false;


    return hasType(resource.ilwisType(),itSCRIPT);
}

bool PythonObjectFactory::prepare()
{
    return true;
}
