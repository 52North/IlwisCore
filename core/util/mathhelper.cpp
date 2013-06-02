#include "kernel.h"
#include "angle.h"
#include "point.h"
#include <Eigen/Dense>
#include "mathhelper.h"

using namespace Ilwis;

MathHelper::MathHelper()
{
}

bool MathHelper::findOblique(int iPoints,
              const std::vector<Coordinate> &independent, const std::vector<Coordinate> &dependent, std::vector<Coefficient> &coef, bool useCols)

{
    if (iPoints < 4) return -2;
    int N = 2 * iPoints;
    int M = 8;

    Eigen::Matrix2d A(N, M);
    Eigen::VectorXd b(N);

    for (int i = 0; i < iPoints; ++i) {
        for (int j = 0; j < 8; ++j) {
            A(2*i,j) = 0;
            A(2*i+1,j) = 0;
        }
        A(2*i, 0) = independent[i].x();
        A(2*i, 1) = independent[i].y();
        A(2*i, 2) = 1;
        A(2*i, 6) = - dependent[i].x() * independent[i].x();
        A(2*i, 7) = - dependent[i].x() * independent[i].y();
        A(2*i+1  , 3) = independent[i].x();
        A(2*i+1  , 4) = independent[i].y();
        A(2*i+1  , 5) = 1;
        A(2*i+1  , 6) = - dependent[i].y() * independent[i].x();
        A(2*i+1  , 7) = - dependent[i].y() * independent[i].y();
        b(2*i) = dependent[i].x();
        b(2*i+1  ) = dependent[i].y();
    }
    auto sol = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
    for (int i = 0; i < 8; ++i) {
        //coef[i] = sol;
    }
    return true;
}

bool MathHelper::findPolynom(int iTerms, int iPoints, const std::vector<Coordinate> &independent, const std::vector<Coordinate> &dependent, std::vector<Coefficient> &coef)
{
    return false;
}
