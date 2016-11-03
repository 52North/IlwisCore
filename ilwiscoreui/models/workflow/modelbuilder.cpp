#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "workflow.h"
#include "operationmetadata.h"
#include "workflowmodel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationcatalogmodel.h"
#include "../workflowerrormodel.h"
#include "ilwiscontext.h"
#include "ilwistypes.h"
#include "applicationsetup.h"
#include "analysispattern.h"
#include "modeldesigner.h"
#include "modelbuilder.h"


ModelBuilder::ModelBuilder(QObject *parent) : QObject(parent)
{

}

ModelDesigner *ModelBuilder::createModel(QObject *parent)
{
    try {
        return new ModelDesigner(parent);
    }
    catch( const ErrorObject& ){}

    return 0;
}
