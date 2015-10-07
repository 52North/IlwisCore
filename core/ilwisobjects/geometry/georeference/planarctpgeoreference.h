#ifndef PLANARCTPGEOREFERENCE_H
#define PLANARCTPGEOREFERENCE_H

namespace Ilwis{



class KERNELSHARED_EXPORT PlanarCTPGeoReference : public CTPGeoReference
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    enum Transformation { tCONFORM, tAFFINE, tSECONDORDER, tFULLSECONDORDER,
                    tTHIRDORDER, tPROJECTIVE, tUNKNOWN } ;
    PlanarCTPGeoReference();
    PlanarCTPGeoReference(const Resource& resource);
    virtual Coordinate pixel2Coord(const Pixeld &pix) const;
    virtual Pixeld coord2Pixel(const Coordinate& crd) const;
    bool isValid() const;
    virtual double pixelSize() const;
    virtual bool compute();
    void transformation(Transformation tr);
    Transformation transformation() const;
    GeoRefImplementation *clone();
    static QString typeName();

    static GeoRefImplementation *create();
private:
    Eigen::Matrix2d _jacobian;
    double _sigma;
    Transformation _transformation;
    std::vector<Coefficient> _colrowCoef;
    std::vector<Coefficient> _xyCoef;
    Coefficient _avgCrd;
    Coefficient _avgPix;

    Coordinate crdInverseOfAffine(const Pixeld &pix) const;
    Coordinate crdInverseOfProjective(const Pixeld &pix) const;
    Coordinate crdInverseOfHigherOrder(const Pixeld &pix);
    void makeJacobianMatrix(const Coordinate &crdIn, Eigen::Matrix2d &rmJ);
    quint32 minnr() const;
    void copyTo(GeoRefImplementation *impl);


};
}

#endif // PLANARCTPGEOREFERENCE_H
