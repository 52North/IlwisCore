#ifndef SELECTIONBASE_H
#define SELECTIONBASE_H

namespace Ilwis {

class SPFeatureI;
class RasterCoverage;
typedef IlwisData<RasterCoverage> IRasterCoverage;

namespace BaseOperations {



class SelectionBase : public OperationImplementation
{
public:
    SelectionBase();

    geos::geom::Point *pixel2point(const Pixel &pix);
protected:
    struct ExpressionPart{

        enum PartType{ptBOX, ptENVELOPE, ptPOLYGON, ptATTRIBUTE, ptATTRIBUTESELECTION, ptFEATURETYPE, ptBANDS};
        ExpressionPart(const Ilwis::ICoverage &attribute, const QString& part);
        bool match(const SPFeatureI& feature,SelectionBase *operation) const;
        bool match(const Pixel& location, double pixelValue,SelectionBase *operation) const;
        QVariant _rightSide;
        int _leftSide; // index of attribute
        PartType _type;
        LogicalOperator _operator = loNONE;
        LogicalOperator _andor = loNONE;
        std::shared_ptr<geos::geom::Geometry> _polygon;
        Envelope _envelope;
        BoundingBox _box;
        QStringList _attributes;
        std::vector<QString> _bands;
        bool _isValid;
        IlwisTypes _geometryType = itFEATURE;

    protected:
        bool checkForBounds(const Pixel &location) const;
    private:
        void setEnvelopePolygon(const IRasterCoverage& raster);
    };

    std::vector<ExpressionPart> _expressionparts;
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    ITable _attTable;
    ITable _inputAttributeTable;
    static std::unique_ptr<geos::geom::GeometryFactory> _geomfactory;

    SelectionBase(quint64 metaid, const Ilwis::OperationExpression &expr);
    void parseSelector(QString select, const Ilwis::ICoverage &coverage);
    BoundingBox boundingBox(const Ilwis::IRasterCoverage &raster) const;
    std::vector<QString> bands(const Ilwis::IRasterCoverage &raster) const;
    QStringList attributeNames() const;
    std::vector<int> organizeAttributes();
    QVariant attribute(double pixelValue, int columnIndex);
};
}
}

#endif // SELECTIONBASE_H
