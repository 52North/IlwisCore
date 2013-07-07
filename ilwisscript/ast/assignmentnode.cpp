#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "kernel.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "symboltable.h"
#include "assignmentnode.h"
#include "formatter.h"
#include "formatters.h"
#include "scriptnode.h"
#include "raster.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "ilwisdata.h"
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

void AssignmentNode::setResult(IDNode *node)
{
    _result =QSharedPointer<IDNode>(node);
}

void AssignmentNode::setFormatPart(ASTNode *node)
{
    _typemodifier = QSharedPointer<ASTNode>(node);
}

void AssignmentNode::setExpression(ExpressionNode *node)
{
    _expression =QSharedPointer<ExpressionNode>(node);
}

QString AssignmentNode::nodeType() const
{
    return "assignment";
}

bool AssignmentNode::evaluate(SymbolTable& symbols, int scope)
{
    if ( _expression.isNull())
        return false;

    QString format, fnamespace;
    bool res = _expression->evaluate(symbols, scope);
    if ( res) {
        if ( !_typemodifier.isNull()) {
            ASTNode *node = static_cast<ASTNode *>(_typemodifier.data());
            if ( node->noOfChilderen()!= 1)
                return ERROR2(ERR_NO_OBJECT_TYPE_FOR_2, "Output object", "expression");

            Formatter *fnode = static_cast<Formatter *>(node->child(0).data());
            format = fnode->format();
            fnamespace = fnode->fnamespace();
        }
        NodeValue val = _expression->value();
        QString result = _result->id();
        Symbol sym = symbols.getSymbol(val.toString(),SymbolTable::gaREMOVEIFANON);
        IlwisTypes tp = sym.isValid() ? sym._type : itUNKNOWN;
        if (  tp & itCOVERAGE) {
            Ilwis::ICoverage coverage;
            if ( tp & itGRID)
                coverage = sym._var.value<Ilwis::IGridCoverage>().get<Coverage>();
            else
                coverage = sym._var.value<Ilwis::IFeatureCoverage>().get<Coverage>();

            if ( format == "" || format == sUNDEF) {
                Formatter *fnode = ScriptNode::activeFormat(itGRID);
                if ( fnode) {
                    format = fnode->format();
                    fnamespace = fnode->fnamespace();
                }
            }
            coverage->setName(result);
            if ( format != "" && format != sUNDEF) {
                coverage->connectTo(QUrl(), format, fnamespace, Ilwis::IlwisObject::cmOUTPUT);
                coverage->setCreateTime(Ilwis::Time::now());
                coverage->store(Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA);

            }
            if ( result.indexOf(INTERNAL_PREFIX) == -1) {
                mastercatalog()->addItems({coverage->source()});
            }
        } else {
            sym = symbols.getSymbol(_result->id(),SymbolTable::gaREMOVEIFANON);
            tp = sym.isValid() ? sym._type : itUNKNOWN;
            if ( tp == itUNKNOWN) {
                tp = Domain::ilwType(val);
            }
        }
        symbols.addSymbol(_result->id(), scope, tp, _expression->value());

        return true;
    }
    return false;
}

