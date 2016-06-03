#ifndef SELECTIONFEATURES_H
#define SELECTIONFEATURES_H

#include "geos/geom/geometry.h"
#include "geometries.h"
#include <memory>

namespace Ilwis {
namespace BaseOperations {

class SelectionFeatures : public OperationImplementation
{
public:
    SelectionFeatures();
    SelectionFeatures(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~SelectionFeatures();

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    struct ExpressionPart{

        enum PartType{ptBOX, ptPOLYGON, ptATTRIBUTE, ptATTRIBUTESELECTION};
        ExpressionPart(const Ilwis::ITable &attribute, const QString& part);
        bool match(const SPFeatureI& feature,SelectionFeatures *operation) const;
        QVariant _rightSide;
        QString _leftSide;
        PartType _type;
        LogicalOperator _operator = loNONE;
        LogicalOperator _andor = loNONE;
        std::shared_ptr<geos::geom::Geometry> _polygon;
        Envelope _envelope;
        QStringList _attributes;
        bool _isValid;
    };

    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    ITable _attTable;
    bool _asIndex = false;

    std::vector<ExpressionPart> _expressionparts;

    NEW_OPERATION(SelectionFeatures);

 };
}
}

#endif // SELECTIONFEATURES_H
