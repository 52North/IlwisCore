#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/modellerfactory.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "smce.h"

using namespace Ilwis;
using namespace Smce;

REGISTER_ANALYSISPATTERN(SMCE)

SMCE::SMCE() : Ilwis::AnalysisPattern()
, _tree(0)
{
    Init();
}

void SMCE::Init()
{
    _tree = new Node(this);
    _tree->setGoal("New Goal ...");
}

bool SMCE::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
    QString script("import ilwis\n");
    script += _tree->getPython("standardized", true);
    if (_tree->fileName().endsWith(".mpr"))
        script += QString("standardized.store('" + _tree->fileName() + "', 'map', 'ilwis3')\n");
    else if (_tree->fileName().endsWith(".tif"))
        script += QString("standardized.store('" + _tree->fileName() + "', 'GTiff', 'gdal')\n");
    else if (_tree->fileName().endsWith(".img"))
        script += QString("standardized.store('" + _tree->fileName() + "', 'HFA', 'gdal')\n");
    else {
        _tree->setFileName(_tree->fileName() + ".mpr");
        script += QString("standardized.store('" + _tree->fileName() + "', 'map', 'ilwis3')\n");
    }
    QString expr = QString("runpython(\"%1\")").arg(script);
    qDebug() << expr;
    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    return Ilwis::commandhandler()->execute(expr, &ctx, syms);
}

IWorkflow SMCE::workflow(const IOOptions &opt)
{
    return IWorkflow();
}

Node * SMCE::root() const
{
    return _tree;
}

void SMCE::store(QDataStream &stream)
{
    stream << type();
    AnalysisPattern::store(stream);
    _tree->store(stream);
}

void SMCE::loadMetadata(QDataStream &stream)
{
    AnalysisPattern::loadMetadata(stream);
    _tree = Node::load(stream, this);
}

QString SMCE::type() const
{
    return "smce";
}

SMCE::SMCE(const QString &name, const QString &description) : AnalysisPattern(name, description)
, _tree(0)
{
    Init();
}

SMCE::AnalysisPattern *SMCE::create(const QString &name, const QString &description, const IOOptions &options)
{
    SMCE *smce =  new SMCE(name, description);
    return smce;
}


