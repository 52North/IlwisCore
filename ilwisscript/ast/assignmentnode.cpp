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
#include "formatnode.h"
#include "modifierpart.h"
#include "scriptnode.h"
#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "coverage.h"
#include "georeference.h"
#include "connectorinterface.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "grid.h"
#include "gridcoverage.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "juliantime.h"


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

void AssignmentNode::setTypeModifierPart(ASTNode *node)
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
            ModifierPart *tnode = static_cast<ModifierPart *>(_typemodifier.data());
            format = tnode->format();
            fnamespace = tnode->fnamespace();
        }
        NodeValue val = _expression->value();
        if ( val.typeName() == QString("Ilwis::IGridCoverage")) {
            if ( format == "" || format == sUNDEF) {
                FormatNode *fnode = ScriptNode::activeFormat(itGRIDCOVERAGE);
                if ( fnode) {
                    format = fnode->format();
                    fnamespace = fnode->fnamespace();
                }
            }
            if ( format == "" || format == sUNDEF) {
                return false;
            }
            Ilwis::IGridCoverage gcresult = val.value<Ilwis::IGridCoverage>();
            QString result = _result->id();
            gcresult->setName(result);
            gcresult->connectTo(QUrl(), format, fnamespace, Ilwis::IlwisObject::cmOUTPUT);
            gcresult->setCreateTime(Ilwis::Time::now());
            gcresult->store(Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA);
        }
        symbols.addSymbol(_result->id(), scope, 0, _expression->value())  ;
        return true;
    }
    return false;
}


