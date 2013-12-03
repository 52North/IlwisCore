#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "symboltable.h"
#include "formatter.h"
#include "formatters.h"
#include "scriptnode.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "connectorinterface.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "juliantime.h"
#include "selectornode.h"
#include "outparametersnode.h"
#include "assignmentnode.h"

using namespace Ilwis;

AssignmentNode::AssignmentNode() : _defintion(false)
{
}

bool AssignmentNode::isDefinition() const
{
    return _defintion;
}

void AssignmentNode::setDefintion(bool yesno)
{
    _defintion= yesno;
}

void AssignmentNode::setExpression(ExpressionNode *node)
{
    _expression =QSharedPointer<ExpressionNode>(node);
}

QString AssignmentNode::nodeType() const
{
    return "assignment";
}

IIlwisObject AssignmentNode::getObject(const Symbol& sym) const {
    IlwisTypes tp = sym._type;
    if ( tp & itRASTER)
            return sym._var.value<Ilwis::IRasterCoverage>().get<IlwisObject>();
    if ( tp & itFEATURE)
            return sym._var.value<Ilwis::IFeatureCoverage>().get<IlwisObject>();
    return IIlwisObject();

}

void AssignmentNode::getFormat(QSharedPointer<ASTNode>& node, QString& format, QString& fnamespace) const {
    Formatter *fnode = static_cast<Formatter *>(node->child(0).data());
    format = fnode->format();
    fnamespace = fnode->fnamespace();

    if ( format == "" || format == sUNDEF) {
        Formatter *fnode = ScriptNode::activeFormat(itRASTER);
        if ( fnode) {
            format = fnode->format();
            fnamespace = fnode->fnamespace();
        }
    }
}

void AssignmentNode::store2Format(QSharedPointer<ASTNode>& node, const Symbol& sym, const QString& result) {
    QString format, fnamespace;
    getFormat(node, format, fnamespace);
    if ( format != "" && format != sUNDEF) {
        Ilwis::IIlwisObject object = getObject(sym);
        bool wasAnonymous = object->isAnonymous();
        QUrl url = context()->workingCatalog()->location().toString() + "/" + result;
        object->setName(result);
        if(!object->connectTo(url, format, fnamespace, Ilwis::IlwisObject::cmOUTPUT))
            return;
        object->setCreateTime(Ilwis::Time::now());
        if ( wasAnonymous)
            mastercatalog()->addItems({object->source(IlwisObject::cmOUTPUT | IlwisObject::cmEXTENDED)});

        object->store(Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA);

     }
}

bool AssignmentNode::evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx)
{
    if ( _expression.isNull())
        return false;


    bool res = _expression->evaluate(symbols, scope, ctx);
    if ( res) {
        NodeValue val = _expression->value();
        for(int i = 0; i < val.size(); ++i) {
            Symbol sym = symbols.getSymbol(val.id(i),SymbolTable::gaREMOVEIFANON);
            IlwisTypes tp = sym.isValid() ? sym._type : itUNKNOWN;
            QString result = _outParms->id(i);
            QSharedPointer<ASTNode> specifier = _outParms->specifier(result);
            if ( !specifier.isNull()) {
                if ( specifier->noOfChilderen()!= 1)
                    return ERROR2(ERR_NO_OBJECT_TYPE_FOR_2, "Output object", "expression");
                store2Format(specifier, sym, result);

            }
            if (  hasType(tp, itILWISOBJECT)) {
                bool ok;
                if ( hasType(tp, itRASTER)) {
                    ok = copyObject<RasterCoverage>(sym, result,symbols);
                }
                else if hasType(tp, itFEATURE)
                        ok = copyObject<FeatureCoverage>(sym, result,symbols);
                else if hasType(tp, itTABLE){
                    ok = copyObject<Table>(sym, result,symbols,true);
                    QSharedPointer<Selector> selector = _outParms->selector(result);
                    if (!selector.isNull()){
                        QString varName = selector->variable();
                        ITable source =  sym._var.value<ITable>();
                        QString oldColName = ctx->_additionalInfo[source->name()];
                        QVariant newT= symbols.getValue(result);
                        ITable newTable = newT.value<ITable>();
                        ColumnDefinition coldef = newTable->columndefinition(oldColName);
                        if ( coldef.isValid()){
                            coldef.setName(varName);
                            newTable->columndefinition(coldef);
                        }
                    }
                }

                if(!ok) {
                    throw ErrorObject(QString(TR(ERR_OPERATION_FAILID1).arg("assignment")));
                }
                ctx->clear(true);
                ctx->_results.push_back(result);
                return ok;

            } else {
                sym = symbols.getSymbol(result,SymbolTable::gaREMOVEIFANON);
                tp = sym.isValid() ? sym._type : itUNKNOWN;
                if ( tp == itUNKNOWN) {
                    tp = Domain::ilwType(val);
                }
            }
            symbols.addSymbol(result, scope, tp, _expression->value());
            //symbols.addSymbol(_result->id(), scope, tp, sym._var);

            return true;
        }
    }
    return false;
}

void AssignmentNode::addOutputs(OutParametersNode *p)
{
    _outParms.reset(p);
}

