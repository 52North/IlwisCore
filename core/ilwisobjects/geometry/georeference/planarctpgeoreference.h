#ifndef PLANARCTPGEOREFERENCE_H
#define PLANARCTPGEOREFERENCE_H

namespace Ilwis{



class PlanarCTPGeoReference : public CTPGeoReference
{
public:
    enum Transformation { tCONFORM, tAFFINE, tSECONDORDER, tFULLSECONDORDER,
                    tTHIRDORDER, tPROJECTIVE } ;
    PlanarCTPGeoReference();
    PlanarCTPGeoReference(const Resource& res);
    virtual Coordinate pixel2Coord(const Pixel_d &pix) const;
    virtual Pixel_d coord2Pixel(const Coordinate& crd) const;
    bool isValid() const;
    virtual double pixelSize() const;
    virtual bool compute();

private:
    Eigen::Matrix2d _jacobian;
    double _sigma;
    Transformation _transformation;
    std::vector<Coefficient> _colrowCoef;
    std::vector<Coefficient> _xyCoef;
    Coefficient _avgCrd;
    Coefficient _avgPix;

    Coordinate crdInverseOfAffine(const Pixel_d &pix) const;
    Coordinate crdInverseOfProjective(const Pixel_d &pix) const;
    Coordinate crdInverseOfHigherOrder(const Pixel_d &pix);
    void makeJacobianMatrix(const Coordinate &crdIn, Eigen::Matrix2d &rmJ);
    quint32 minnr() const;


};
}

#endif // PLANARCTPGEOREFERENCE_H
