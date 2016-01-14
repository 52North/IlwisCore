#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

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
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "connectorinterface.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "juliantime.h"
#include "selectnode.h"
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

void AssignmentNode::setExpression(ASTNode *node)
{
    _expression =QSharedPointer<ASTNode>(node);
}

QString AssignmentNode::nodeType() const
{
    return "assignment";
}

IIlwisObject AssignmentNode::getObject(const Symbol& sym) const {
    IlwisTypes tp = sym._type;
    if ( tp & itRASTER)
        return sym._var.value<Ilwis::IRasterCoverage>().as<IlwisObject>();
    if ( tp & itFEATURE)
        return sym._var.value<Ilwis::IFeatureCoverage>().as<IlwisObject>();
    if ( hasType(tp , itTABLE|itCOLUMN))
        return sym._var.value<Ilwis::ITable>().as<IlwisObject>();
    if ( hasType(tp , itDOMAIN))
        return sym._var.value<Ilwis::IDomain>().as<IlwisObject>();
    if ( hasType(tp , itGEOREF))
        return sym._var.value<Ilwis::IGeoReference>().as<IlwisObject>();
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
    QString format, provider;
    getFormat(node, format, provider);
    if ( format != "" && format != sUNDEF) {
        Ilwis::IIlwisObject object = getObject(sym);
        if ( object.isValid()){
            bool wasAnonymous = object->isAnonymous(); // if object is anonymous it will get a name due this method; this means it will now appear in the mastercatalog
            // as (previous) anonymous objects are not in the mastercatalog ( though they are registered)
            QString name = result;
            QUrl url;
            if ( result.indexOf(":/") != -1 && result.indexOf("//") != -1) {// is already an url, than we figure out its name from the url
                url = result;
                name = result.mid(result.lastIndexOf("/") + 1);
            }
            else
                // no path information so we create our own path, the name has no path information so can be used as is
                if ( provider != "stream"){ // stream goes to the internal if nothing has ben defined and that is default.
                    url = context()->workingCatalog()->source().url().toString() + "/" + result;
                }else {
                    url = context()->persistentInternalCatalog().toString() + "/" + result;
                }
            object->name(name);
            // we reuse an existing connector if it is of the same provider; it will than inherit/use properties of the "old" connector
            if ( object->provider() != provider)
                object->connectTo(url, format, provider, Ilwis::IlwisObject::cmOUTPUT);
            object->createTime(Ilwis::Time::now());
            if ( wasAnonymous)
                mastercatalog()->addItems({object->source(IlwisObject::cmOUTPUT | IlwisObject::cmEXTENDED)});

            object->store({"storemode",Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA});
        }else {
            kernel()->issues()->log(QString(TR("Couldn't retrieve symbol from symbol table, object will not be stored")));
        }

    }
}

bool AssignmentNode::evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx)
{
    if ( _expression.isNull())
        return false;

    try{
        bool ok = _expression->evaluate(symbols, scope, ctx);
        if ( ok) {
            // we save the additional info  as we might need it but for the rest clear
            // the results as the result of the assignment node is a newly filled ctx
            auto additionalInfo = ctx->_additionalInfo;
            ctx->clear(true);

            NodeValue val = _expression->value();
            for(int i = 0; i < val.size(); ++i) {
                Symbol sym = symbols.getSymbol(val.id(i),SymbolTable::gaREMOVEIFANON);
                IlwisTypes tp = sym.isValid() ? sym._type : itUNKNOWN;
                QString result = _outParms->id(i);

                if (  hasType(tp, itILWISOBJECT | itCOLUMN)) {

                    if ( hasType(tp, itRASTER)) {
                        ok &= copyObject<RasterCoverage>(sym, result,symbols);
                    }
                    else if (hasType(tp, itFEATURE))
                        ok &= copyObject<FeatureCoverage>(sym, result,symbols);
                    else if ( hasType(tp, itDOMAIN)){
                        ok &= copyObject<Domain>(sym, result,symbols);
                    } else if ( hasType(tp, itGEOREF)){
                        ok &= copyObject<GeoReference>(sym, result,symbols);
                    } else if (hasType(tp, itTABLE | itCOLUMN)){
                        ok &= copyObject<Table>(sym, result,symbols,true);
                        QSharedPointer<Selector> selector = _outParms->selector(result);
                        if (!selector.isNull()){
                            QString varName = selector->variable();
                            ITable source =  sym._var.value<ITable>();
                            QString oldColName = additionalInfo[source->name()].toString();
                            QVariant newT= symbols.getValue(result);
                            ITable newTable = newT.value<ITable>();
                            ColumnDefinition& coldef = newTable->columndefinitionRef(oldColName);
                            if ( coldef.isValid()){
                                coldef.name(varName);
                            }
                        }
                    }

                    if(!ok) {
                        throw ErrorObject(QString(TR(ERR_OPERATION_FAILID1).arg("assignment")));
                    }
                    QSharedPointer<ASTNode> specifier = _outParms->specifier(_outParms->id(i));
                    if ( !specifier.isNull()) {
                        if ( specifier->noOfChilderen()!= 1)
                            return ERROR2(ERR_NO_OBJECT_TYPE_FOR_2, "Output object", "expression");
                        store2Format(specifier, sym, result);

                    }

                    ctx->_results.push_back(result);

            } else {
                sym = symbols.getSymbol(result,SymbolTable::gaREMOVEIFANON);
                tp = sym.isValid() ? sym._type : itUNKNOWN;
                if ( tp == itUNKNOWN) {
                    tp = Domain::ilwType(val);
                }
            }
            //ctx->addOutput(symbols,_expression->value(),result, tp, Resource());

        }
    }
    return ok;
} catch(const ErrorObject&){

}

return false;
}

void AssignmentNode::addOutputs(OutParametersNode *p)
{
    _outParms.reset(p);
}

void AssignmentNode::setOutId(IDNode *idnode)
{
    _outParms.reset(new OutParametersNode());
    _outParms->addResult(idnode);

}


