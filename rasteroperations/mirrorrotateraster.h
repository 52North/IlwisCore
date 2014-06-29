#ifndef TRANSPOSERASTER_H
#define TRANSPOSERASTER_H

namespace Ilwis {
namespace RasterOperations {

class MirrorRotateRaster : public OperationImplementation
{
public:
    enum TransPoseMethod{tmMirrorHorizontal, tmMirrorVertical, tmMirrorDiagonal, tmRotata90, tmRotate180, tmRotate270, tmTranspose};
    MirrorRotateRaster();

    MirrorRotateRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(MirrorRotateRaster);

private:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    TransPoseMethod _method;

    void mirrorvertical(const Ilwis::BoundingBox &box);
};
}
}

#endif // TRANSPOSERASTER_H
