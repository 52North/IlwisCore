#ifndef SELECTIONBASE_H
#define SELECTIONBASE_H

namespace Ilwis {

class SPFeatureI;

namespace BaseOperations {



class SelectionBase : public OperationImplementation
{
public:
    SelectionBase();

protected:
    struct ExpressionPart{

        enum PartType{ptBOX, ptENVELOPE, ptPOLYGON, ptATTRIBUTE, ptATTRIBUTESELECTION, ptFEATURETYPE, ptBANDS};
        ExpressionPart(const Ilwis::ICoverage &attribute, const QString& part);
        bool match(const SPFeatureI& feature,SelectionBase *operation) const;
        QVariant _rightSide;
        int _leftSide; // index of attribute
        PartType _type;
        LogicalOperator _operator = loNONE;
        LogicalOperator _andor = loNONE;
        std::shared_ptr<geos::geom::Geometry> _polygon;
        Envelope _envelope;
        BoundingBox _box;
        QStringList _attributes;
        std::vector<int> _bands;
        bool _isValid;
        IlwisTypes _geometryType = itFEATURE;
    };

    std::vector<ExpressionPart> _expressionparts;

    SelectionBase(quint64 metaid, const Ilwis::OperationExpression &expr);
    void parseSelector(QString select, const Ilwis::ITable &attTable);
    int numberOfBandsInSelection() const;
    BoundingBox boundingBox() const;
};
}
}

#endif // SELECTIONBASE_H
