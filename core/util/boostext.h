#ifndef BOOSTEXT_H
#define BOOSTEXT_H

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>


namespace boostnum = boost::numeric::ublas;
namespace ilwis {
/* Matrix inversion routine.
   Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
template<class T>
bool inv(const boostnum::matrix<T>& input, boostnum::matrix<T>& inverse) {

   using namespace boost::numeric::ublas;
   typedef permutation_matrix<std::size_t> pmatrix;
   // create a working copy of the input
   matrix<T> A(input);
   // create a permutation matrix for the LU-factorization
   pmatrix pm(A.size1());
   // perform LU-factorization
   int res = lu_factorize(A,pm);
       if( res != 0 ) return false;
   // create identity matrix of "inverse"
   inverse.assign(boostnum::identity_matrix<T>(A.size1()));
   // backsubstitute to get the inverse
   lu_substitute(A, pm, inverse);
   return true;
}

template<class T> double det( const boostnum::matrix<T>& mc ) {
    boostnum::matrix<T>& m = const_cast<boostnum::matrix<T>& >(mc); // not ideal but has to do with definitions inside boost
    boostnum::permutation_matrix< std ::size_t> pm(m.size1());
    double det = 1.0;
    if( boostnum::lu_factorize(m,pm) ) {
        det = 0.0;
    } else {
        for(size_t i = 0; i < m.size1(); i++)
            det *= m(i,i); // multiply by elements on diagonal
        int pm_sign=1;
        std::size_t size = pm.size();
        for (std::size_t i = 0; i < size; ++i)
            if (i != pm(i))
                pm_sign *= -1.0;
        det = det * pm_sign;
    }
    return det;
}


}

#endif // BOOSTEXT_H
