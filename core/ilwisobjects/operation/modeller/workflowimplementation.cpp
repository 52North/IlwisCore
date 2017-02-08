#include <functional>
#include <future>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "table.h"
#include "raster.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "executionnode.h"
#include "workflowimplementation.h"

using namespace Ilwis;

WorkflowImplementation::WorkflowImplementation()
{

}

WorkflowImplementation::WorkflowImplementation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

template<class T> void setOutput(const QVariant& value,ExecutionContext *ctx, SymbolTable &symTable){
    T obj = value.value<T>();
    ctx->addOutput(symTable,value,obj->name(), obj->ilwisType(), obj->resource() );
}

void WorkflowImplementation::clearCalculatedValues(){
    for(auto& node : _nodes){
        node.second.clearCalculatedValues();
    }
}

void WorkflowImplementation::initStepMode(){
    QThread *current = QThread::currentThread();
    QVariant var = current->property("runparameters");
    if ( var.isValid()){
        QVariantMap values = var.toMap();
        _stepMode = values["stepmode"].toBool();
        if ( _stepMode){
            bool ok;
            quint32 id = values["runid"].toUInt(&ok);
            if ( ok){
                _runid = id;
                kernel()->addSyncLock(_runid);
            }else
                _stepMode = false;
        }
    }
}

bool WorkflowImplementation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    auto removeLock = [](qint32 runid)->void {
        if ( runid != iUNDEF){
            kernel()->removeSyncLock(runid)        ;
        }
    };

    connect(this, &WorkflowImplementation::sendMessage, kernel(),&Kernel::acceptMessage);
    connect(kernel(), &Kernel::sendMessage, this, &WorkflowImplementation::acceptMessage);
    _stopExecution = false;
    _initial = true;
    initStepMode();

    ExecutionContext ctx2;
    SymbolTable symTable2;
    std::vector<SPWorkFlowNode> nodes = _workflow->outputNodes();
    clearCalculatedValues();
    for(SPWorkFlowNode node : nodes ) {
        if ( _stopExecution){
            removeLock(_runid);
            return false;
        }
        ExecutionNode& exnode = executionNode(node);
        if(!exnode.execute(&ctx2, symTable2, this, _expression, _workflow->parmid2order())){
            removeLock(_runid);
            return false;
        }
    }
    for(int i=0; i < ctx2._results.size(); ++i){

        Symbol sym = symTable2.getSymbol(ctx2._results[i]);
        QVariant value = sym._var;
        if ( hasType(sym._type, itTABLE)){
            setOutput<ITable>(value, ctx, symTable);
        }else  if ( hasType(sym._type, itRASTER)){
            setOutput<IRasterCoverage>(value, ctx, symTable)   ;
        }else if ( hasType(sym._type, itFEATURE)){
            setOutput<IFeatureCoverage>(value, ctx, symTable)   ;
        }else if ( hasType(sym._type, itNUMBER)){
             ctx->addOutput(symTable, QVariant(value), sUNDEF, itDOUBLE, Resource());
        }else if ( hasType(sym._type, itSTRING)){
            ctx->addOutput(symTable, QVariant(value), sUNDEF, itSTRING, Resource());
       }
    }
    removeLock(_runid);

    return true;
}

Ilwis::OperationImplementation *WorkflowImplementation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new WorkflowImplementation(metaid, expr);
}

Ilwis::OperationImplementation::State WorkflowImplementation::prepare(ExecutionContext *ctx, const SymbolTable &)
{

    QString url = "ilwis://operations/" + _expression.name() + ".ilwis";
    quint64 id = mastercatalog()->name2id(url, itWORKFLOW);
    if ( id == i64UNDEF){
        kernel()->issues()->log(TR("Couldnt find a valid workflow for ") + _expression.name());
        return sPREPAREFAILED;
    }
    if (!_workflow.prepare(id)){
        kernel()->issues()->log(TR("Couldnt find a valid workflow for ") + _expression.name());
        return sPREPAREFAILED;
    }
    return sPREPARED;
}

ExecutionNode &WorkflowImplementation::executionNode(const SPWorkFlowNode &node)
{
    auto iter = _nodes.find(node->id());
    if ( iter == _nodes.end()){
        _nodes[node->id()] = ExecutionNode(node);
        return _nodes[node->id()];
    }
    return (*iter).second;
}

void WorkflowImplementation::wait(const SPWorkFlowNode& node)
{

    if (_stepMode){
        bool ok;
        QVariantMap mp;
        mp["node"] = node->id();
        mp["id"] = _metadata->id();
        mp["runid"] = _runid;
        mp["condtionid"] = node->conditionIdOfTest();
        emit sendMessage("workflow","currentnode",mp);

        if ( !_initial){
            QWaitCondition& waitc = kernel()->waitcondition(_runid, ok);
            if ( ok){
                _syncMutex.lock();
                waitc.wait(&_syncMutex);
            }
        }

    }
}

void WorkflowImplementation::wakeup()
{
    if ( _stepMode){
        if (_initial){
            _initial = false;
            return;
        }

        _syncMutex.unlock();
    }
}

void WorkflowImplementation::acceptMessage(const QString &type, const QString &subtype, const QVariantMap &parameters)
{
    if ( type == "workflow"){
        Locker<std::mutex> lock(_lock);
        bool ok;
        quint64 id = parameters["runid"].toLongLong(&ok);
        if ( ok && id == _runid){ // check if this was meant for this workflow
            if ( subtype == "stepmode"){
                _stepMode = parameters.contains("id") ? parameters["stepmode"].toBool() : false;
                if ( !_stepMode){
                    _syncMutex.unlock();
                }
            }else if ( subtype == "stopstepmode"){
                _stepMode = false;
                _stopExecution = true;
                clearCalculatedValues();
            }
        }
    }
}

void WorkflowImplementation::sendData(NodeId nodeId,ExecutionContext *ctx, SymbolTable &symTable)
{
    if ( _stepMode){
        QVariantList data;
        QVariantMap generic;
        generic["id"] = _workflow->id();
        generic["runid"] = _runid;
        for(int i=0; i<ctx->_results.size(); ++i){
            QVariantMap opdata;
            QVariant var = symTable.getValue(ctx->_results[i]);
            opdata["value"] = var;
            IlwisTypes tp = symTable.ilwisType(QVariant(),ctx->_results[i]);
            opdata["type"] = tp;
            opdata["resultname"] = ctx->_results[i];
            opdata["node"] = nodeId;
            IOperationMetaData meta = _workflow->nodeById(nodeId)->operation();
            QString name =  meta.isValid() ? QString("%2=%1(%3)").arg(meta->name()).arg(nodeId).arg(i) : "";
            opdata["name"] = name;
            IIlwisObject obj = OperationHelper::variant2ilwisobject(var, tp);
            opdata["id"] = obj.isValid() ? obj->id() : i64UNDEF;

            data.append(opdata);
        }
        generic["results"] = data;
        emit sendMessage("workflow","outputdata",generic);
    }
}

bool WorkflowImplementation::stopExecution() const
{
    return _stopExecution;
}



