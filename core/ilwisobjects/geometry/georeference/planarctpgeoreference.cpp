#include <QString>

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "controlpoint.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "ctpgeoreference.h"
#include "eigen3/Eigen/LU"
#include "mathhelper.h"
#include "planarctpgeoreference.h"

using namespace Ilwis;

GeoRefImplementation *PlanarCTPGeoReference::create()
{
    return new PlanarCTPGeoReference();
}

PlanarCTPGeoReference::PlanarCTPGeoReference() : CTPGeoReference("planartiepoints"), _transformation(tUNKNOWN)
{
}

Coordinate PlanarCTPGeoReference::pixel2Coord(const Pixeld &pix) const
{
    if (!isValid() || !pix.isValid())
        return Coordinate();
    Pixeld pixd = pix;
    pixd -= { _avgPix._x, _avgPix._y};
    Coordinate c(_avgCrd._x, _avgCrd._y);
    Coordinate crdInv;
    switch (_transformation) {
    case tAFFINE:
    case tCONFORM:
        crdInv = crdInverseOfAffine(pixd);
        c +=  {crdInv.x, crdInv.y, crdInv.z};
        break;
    case tPROJECTIVE:
        crdInv = crdInverseOfProjective(pixd);
        c +=  {crdInv.x, crdInv.y, crdInv.z};
        break;
    case tSECONDORDER:
        // rCoeffCol and rCoeffRow are filled with coefficients that assume
        // RowCols and Coords reduced to rAvgRow rAvgX etc (Centers of Gravity)
        // used in the following conversion function:
    case tFULLSECONDORDER:
    case tTHIRDORDER:
        crdInv = const_cast<PlanarCTPGeoReference *>(this)->crdInverseOfHigherOrder(pixd);
        c +=  {crdInv.x, crdInv.y, crdInv.z}; // shift back to true position, (crd was already == crdAvg)
        break;
    default:
        return Coordinate();
    }
    return c;
}

Pixeld PlanarCTPGeoReference::coord2Pixel(const Coordinate &crd) const
{
    if(!(isValid() && crd.isValid()))
        return Pixeld();
    Coordinate c = crd;
    Pixeld rc(_avgPix._x, _avgPix._y);
    c -= {_avgCrd._x, _avgCrd._y};
    double X2, Y2, XY;
    switch (_transformation) {
    case tTHIRDORDER:
    case tFULLSECONDORDER:
        X2 = c.x * c.x;
        Y2 = c.y * c.y;
    case tSECONDORDER:
        XY = c.x * c.y;
        break;
    default:
        break;
    }
    switch (_transformation) {
    case tTHIRDORDER:
        rc += {(_colrowCoef[6]._x * c.x + _colrowCoef[7]._x * c.y) * X2 + (_colrowCoef[8]._x * c.x + _colrowCoef[9]._x * c.y) * Y2,
                (_colrowCoef[6]._y * c.x + _colrowCoef[7]._y * c.y) * X2 +(_colrowCoef[8]._y * c.x + _colrowCoef[9]._x * c.y) * Y2};
    case tFULLSECONDORDER:
        rc += {_colrowCoef[4]._x * X2 + _colrowCoef[5]._x * Y2, _colrowCoef[4]._y * X2 + _colrowCoef[5]._y * Y2};
    case tSECONDORDER:
        rc += {_colrowCoef[3]._x * XY,_colrowCoef[3]._y * XY};
    case tAFFINE:
    case tCONFORM:
        rc += {_colrowCoef[0]._x, _colrowCoef[0]._y};
        rc += {_colrowCoef[1]._x * c.x + _colrowCoef[2]._x * c.y, _colrowCoef[1]._y * c.x + _colrowCoef[2]._y * c.y};
        break;
    case tPROJECTIVE:
        rc.x = rc.x + (_colrowCoef[0]._x * c.x + _colrowCoef[1]._x * c.y + _colrowCoef[2]._x) /
                       (_colrowCoef[6]._y * c.x + _colrowCoef[7]._y * c.y + 1);
        rc.y = rc.y + (_colrowCoef[3]._x * c.x + _colrowCoef[4]._x * c.y + _colrowCoef[5]._x) /
                      (_colrowCoef[6]._y * c.x + _colrowCoef[7]._y * c.y + 1);
        break;
    default:
        return Pixeld();

    }
    return rc;
}

Coordinate PlanarCTPGeoReference::crdInverseOfAffine(const Pixeld& pix) const
{
    // Solving the following Coord2RowCol equations for X and Y:
    // Col = aX + by + c    (I)
    // Row = fX + gY + h   (II)
    // becomes:
    //  a X + b Y = Col - c
    //  f X + g Y = Row - h
    // Using Cramers determinants rule:
    // det = ag - bf ;  detX = (Col -c)g - (Row -h)b
    //    detY = (Row -h)a - (Col -c)f
    // yields X = detX/det and Y = detY / det  provided  det <> 0
    double a = _colrowCoef[1]._x;
    double b = _colrowCoef[2]._y;
    double c = _colrowCoef[0]._x;
    double f = _colrowCoef[1]._y;
    double g = _colrowCoef[2]._y;
    double h = _colrowCoef[0]._y;
    double rDet = a * g - b * f;
    Coordinate crd;
    if (std::abs(rDet) < EPS10)
        return crdUNDEF;
    else  {
        crd.x = ((pix.x - c) * g - (pix.y - h) * b ) / rDet;
        crd.y = ((pix.y - h) * a - (pix.x - c) * f ) / rDet;
    }
    return crd;
}

Coordinate PlanarCTPGeoReference::crdInverseOfProjective(const Pixeld& pix) const
{
    // Solving the following Coord2RowCol equations for X and Y:
    // Col = aX + by + c / ( dX + eY + 1)   (I)
    // Row = fX + gY + h / ( dX + eY + 1)   (II)
    // becomes:
    // ( a - dCol)X + (b - eCol)Y = Col - c
    // ( f - dRow)X + (g - eRow)Y = Row - h
    // compressed into  pX + qX = r  and sX + tY = u
    // Using Cramers determinants rule:
    // det = pt - qs ;  detX = rt - qu  and detY = pu - rs
    // yields X = detX/det and Y = detY / det  provided  det <> 0
    double a = _colrowCoef[0]._x;
    double b = _colrowCoef[1]._x;
    double c = _colrowCoef[2]._x;
    double d = _colrowCoef[6]._x;
    double e = _colrowCoef[7]._x;
    double f = _colrowCoef[3]._x;
    double g = _colrowCoef[4]._x;
    double h = _colrowCoef[5]._x;
    double p = a - d * pix.x;
    double q = b - e * pix.x;
    double r = pix.x  - c;
    double s = f - d * pix.y;
    double t = g - e * pix.y;
    double u = pix.y  - h;
    double rDet = p * t - q * s;
    Coordinate crd;
    if (std::abs(rDet) < EPS10)
        return crdUNDEF;
    else  {
        crd.x = ( r * t - q * u ) / rDet;
        crd.y = ( p * u - r * s ) / rDet;
    }
    return crd;
}


Coordinate PlanarCTPGeoReference::crdInverseOfHigherOrder(const Pixeld& pix)
{
    // Solving the following Coord2RowCol 3rd oreder equations for X and Y:
    // Col = c0 + c1X + c2Y + c3XY + c4XX + c5YY + c6XXX + c7XXY + c8XYY + c9YYY  (I )
    // Row = r0 + r1X + r2Y + r3XY + r4XX + r5YY + r6XXX + r7XXY + r8XYY + r9YYY  (II)
    // 2nd order if c6 == c7 == c8 == c9 == r6 == r7 == r8 == r9 == 0
    Coordinate crdNext = Coordinate(0,0,0);     // initial crd guess
    std::vector<double> nextPix { _colrowCoef[0]._x, _colrowCoef[0]._y }; // initial column and row
    Pixeld deltaPix = pix;
    deltaPix -= nextPix;
    makeJacobianMatrix(crdNext, _jacobian);// linear appr matrix of Coord2RowCol in (0,0)

    double detJ = _jacobian.determinant();
    if (abs(detJ) < EPS10)
        return crdUNDEF;
    else  {
        _jacobian = _jacobian.inverse();
    }
    auto addJac = [&]()->std::vector<double>{
            std::vector<double> delta = {_jacobian(0,0) * deltaPix.x + _jacobian(0,1) * deltaPix.y,
            _jacobian(1,0) * deltaPix.x + _jacobian(1,1) * deltaPix.y,0};
            return delta;
    };

    crdNext += addJac();

    // crdNext is now 1st appr of RowCol2Coord(rRow, rCol,crd)
    double gapStart = pixelSize() * 2;
    Coordinate crdTrue;
    Coordinate crdGap(gapStart,gapStart);
    Coordinate crd0(0,0);
    int iCount = 0;
    while ( !crdGap.distance(crd0) < gapStart * 2 && iCount < 10) {
        makeJacobianMatrix(crdNext, _jacobian);
        detJ = _jacobian.determinant();
        if (std::abs(detJ) < EPS10)
            return crdUNDEF;
        else  {
            _jacobian = _jacobian.inverse();
        }
        deltaPix -= nextPix;
        //improvement of crd found (linear approx) using inverse Jacobian matrix
        crdNext += addJac();
        crdTrue = Coordinate(crdNext.x + _avgCrd._x, crdNext.y + _avgCrd._y);
        nextPix = coord2Pixel(crdTrue);
        crdGap = Coordinate(deltaPix.x, deltaPix.y);
        nextPix[0] -= _avgPix._x;
        nextPix[1] -= _avgPix._y;

        iCount++;
    }
    if (iCount < 10)
        return crdNext;
    return crdUNDEF;
}

void PlanarCTPGeoReference::makeJacobianMatrix(const Coordinate &crdIn , Eigen::Matrix2d& rmJ)
{                     // Jacobian for 2nd/3rd order inversion
     //double	c0 = coeffCol[0];
    double	c1 = _colrowCoef[1]._x;
    double	c2 = _colrowCoef[2]._x;
    double	c3 = _colrowCoef[3]._x;
    double	c4 = _colrowCoef[4]._x;
    double	c5 = _colrowCoef[5]._x;
    double	c6 = _colrowCoef[6]._x;
    double	c7 = _colrowCoef[7]._x;
    double	c8 = _colrowCoef[8]._x;
    double	c9 = _colrowCoef[9]._x;

    double	r1 = _colrowCoef[1]._y;
    double	r2 = _colrowCoef[2]._y;
    double	r3 = _colrowCoef[3]._y;
    double	r4 = _colrowCoef[4]._y;
    double	r5 = _colrowCoef[5]._y;
    double	r6 = _colrowCoef[6]._y;
    double	r7 = _colrowCoef[7]._y;
    double	r8 = _colrowCoef[8]._y;
    double	r9 = _colrowCoef[9]._y;

    double x = crdIn.x;
    double y = crdIn.y;
    rmJ(0,0) = c1 + c3 * y;
    rmJ(0,1) = c2 + c3 * x;
    rmJ(1,0) = r1 + r3 * y;
    rmJ(1,1) = r2 + r3 * x;
    if (_transformation == tFULLSECONDORDER) {
        rmJ(0,0) +=  2 * c4 * x;
        rmJ(0,1) +=  2 * c5 * y;
        rmJ(1,0) +=  2 * r4 * x;
        rmJ(1,1) +=  2 * r5 * y;
    }
    if (_transformation == tTHIRDORDER) {
        double XX = x*x;
        double YX = x*y;
        double YY = y*y;
        rmJ(0,0) +=  3 * c6 * XX  + 2 * c7 * YX  + c8 * YY;
        rmJ(0,1) +=  c7 * XX  + 2 * c8 * YX  + 3 * c9 * YY;
        rmJ(1,0) +=  3 * r6 * XX  + 2 * r7 * YX  + r8 * YY;
        rmJ(1,1) +=  r7 * XX  + 2 * r8 * YX  + 3 * r9 * YY;
    }
}

bool PlanarCTPGeoReference::compute()
{
  int i, iNr;
  int iRecs = nrControlPoints();
  if (iRecs == 0)
      return false;
  int iRes = 0;
  std::vector<Coordinate> crdXY(iRecs);
  std::vector<Coordinate> pixColRow(iRecs);
  _colrowCoef.resize(10);
  _xyCoef.resize(10);
  double rSumX = 0;
  double rSumY = 0;
  double rSumRow = 0;
  double rSumCol = 0;
  iNr = 0;
  for (i = 0; i < iRecs; ++i) {
    if (controlPoint(i).isActive()) {
      bool fAct = true;
      double r = controlPoint(i).x;
      if (r == rUNDEF || std::abs(r) > 1e20)
        fAct = false;
      if (fAct) {
        r = controlPoint(i).y;
        if (r == rUNDEF || std::abs(r) > 1e20)
          fAct = false;
      }
      if (fAct) {
          r = controlPoint(i).gridLocation().y;
        if (r == rUNDEF || std::abs(r) > 1e6)
          fAct = false;
      }
      if (fAct) {
        r = controlPoint(i).gridLocation().x;
        if (r == rUNDEF || std::abs(r) > 1e6)
          fAct = false;
      }
      if (!fAct)
        controlPoint(i).active(false);
      else {
        crdXY[iNr].x = controlPoint(i).x;
        crdXY[iNr].y = controlPoint(i).y;
        rSumX += crdXY[iNr].x;
        rSumY += crdXY[iNr].y;
        pixColRow[iNr].x = controlPoint(i).gridLocation().x - 0.5;
        pixColRow[iNr].y = controlPoint(i).gridLocation().y - 0.5;
        rSumCol += pixColRow[iNr].x;
        rSumRow += pixColRow[iNr].y;
        iNr += 1;
      }
    }
  }
  if (iNr == 0)
    iRes = -1;
  else if (iNr < minnr())
    iRes = -2;
  if (0 == iRes) {
    _avgCrd._x = rSumX / iNr;
    _avgCrd._y = rSumY / iNr;
    _avgPix._y = rSumRow / iNr;
    _avgPix._x = rSumCol / iNr;
    for (i = 0; i < iNr; ++i) {
      crdXY[i] -= {_avgCrd._x,_avgCrd._y};
      Coordinate crd = crdXY[i];
      pixColRow[i] -= {_avgPix._x,_avgPix._y};
    }
    if (_transformation == tCONFORM) {
      double sxr, sxc, syr, syc, srr, scc, sxx, syy;
      sxr = sxc = syr = syc = srr = scc = sxx = syy = 0;
      i = 0;
      for (i = 0; i < iNr; ++i) {
          double x = crdXY[i].x;
          double y = crdXY[i].y;
          double c = pixColRow[i].x;
          double r = pixColRow[i].y;
        sxx += x * x;
        syy += y * y;
        scc += c * c;
        srr += r * r;
        sxc += x * c;
        syc += y * c;
        sxr += x * r;
        syr += y * r;
      }
      if (abs(scc + srr) < 1 || abs(sxx + syy) < EPS10)
        iRes = -3;
      else {
        double a = (sxc - syr) / (scc + srr);
        double b = (sxr + syc) / (scc + srr);
        _xyCoef[1]._x =  a;
        _xyCoef[2]._x =  b;
        _xyCoef[1]._x =  b;
        _xyCoef[2]._y = -a;
        _xyCoef[0]._y =  0;
        _xyCoef[0]._y =  0;
        double rDet = a * a + b * b;
        _colrowCoef[1]._x =  a / rDet;
        _colrowCoef[2]._x =  b / rDet;
        _colrowCoef[1]._y =  b / rDet;
        _colrowCoef[2]._y = -a / rDet;
        _colrowCoef[0]._x =  0;
        _colrowCoef[0]._y =  0;
      }
    }
    else if (_transformation == tPROJECTIVE) {
      iRes = MathHelper::findOblique(iNr, pixColRow, crdXY, _colrowCoef, true);
      if (iRes == 0)
        iRes = MathHelper::findOblique(iNr, crdXY, pixColRow, _colrowCoef, false);
    }
    else {
      int iTerms = minnr();
      iRes = MathHelper::findPolynom(iTerms, iNr, crdXY, pixColRow, _colrowCoef);
      if (iRes == 0)
        iRes = MathHelper::findPolynom(iTerms, iNr, pixColRow, crdXY, _xyCoef);

    }
  }
  return isValid();
}

void PlanarCTPGeoReference::transformation(PlanarCTPGeoReference::Transformation tr)
{
    _transformation = tr;
}

PlanarCTPGeoReference::Transformation PlanarCTPGeoReference::transformation() const
{
    return _transformation;
}

GeoRefImplementation *PlanarCTPGeoReference::clone()
{
    GeoRefImplementation  *impl = new PlanarCTPGeoReference();
    copyTo(impl);
    return impl;
}

QString PlanarCTPGeoReference::typeName()
{
    return "planartiepoints";
}

quint32 PlanarCTPGeoReference::minnr() const
{
  switch (_transformation) {
    case tCONFORM:
      return 2;
    case tAFFINE:
      return 3;
    case tSECONDORDER:
      return 4;
    case tFULLSECONDORDER:
      return 6;
    case tTHIRDORDER:
      return 10;
    case tPROJECTIVE:
      return 4;
    default:
      return 0;
  };
}

void PlanarCTPGeoReference::copyTo(GeoRefImplementation *impl)
{
    CTPGeoReference::copyTo(impl);
    PlanarCTPGeoReference *ctpgrf = static_cast<PlanarCTPGeoReference *>(impl);

    ctpgrf->_jacobian = _jacobian;
    ctpgrf->_avgCrd = _avgCrd;
    ctpgrf->_avgPix = _avgPix;
    ctpgrf->_xyCoef = _xyCoef;
    ctpgrf->_colrowCoef = _colrowCoef;
    ctpgrf->_transformation = _transformation;
    ctpgrf->_sigma = _sigma;
}

bool PlanarCTPGeoReference::isValid() const
{
    return CTPGeoReference::isValid() && nrControlPoints() >= minnr();

}

double PlanarCTPGeoReference::pixelSize() const
{
    return 0;
}

