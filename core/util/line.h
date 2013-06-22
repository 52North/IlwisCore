#ifndef LINE_H
#define LINE_H

namespace Ilwis{
template<class PointType=Point2D<double> >
class Line2D  : public std::vector<PointType> {
public:

    Line2D<PointType>& operator+=(const std::vector<double>& vec){
        if (!isValid() || vec.size() != 2)
            return *this;
        for(auto& pnt: *this) {
            pnt.d1( pnt.x() + vec[0]);
            pnt.d2( pnt.y() + vec[1]);
        }

        return *this;
    }

    Line2D<PointType>& operator-=(const std::vector<double>& vec){
        if (!isValid() || vec.size() != 2)
            return *this;
        for(auto& pnt: *this) {
            pnt.d1( pnt.x() - vec[0]);
            pnt.d2( pnt.y() - vec[1]);
        }

        return *this;
    }

    Line2D<PointType>& operator*=(const std::vector<double>& vec){
        if (!isValid() || vec.size()<=2 )
            return *this;
        PointType t1,t2;
        t1 = t2 = vec[0];
        if ( vec.size() == 2)
            t2 = vec[1];
        for(auto& pnt: *this) {
            pnt.d1( pnt.x() * t1);
            pnt.d2( pnt.y() * t2);
        }

        return *this;
    }

    Line2D<PointType>& operator/=(const std::vector<double>& vec){
        if (!isValid() || vec.size()<=2 )
            return *this;
        double t1,t2;
        t1 = t2 = vec[0];
        if ( vec.size() == 2)
            t2 = vec[1];
        if ( t1 == 0 || t2 == 0)
            return *this;
        for(auto& pnt: *this) {
            pnt.d1( pnt.x() / t1);
            pnt.d2( pnt.y() / t2);
        }

        return *this;
    }


    bool isValid() const{
        return this->size() != 0;
    }
private:
    void init() {
        undefined = rUNDEF;
    }
    double undefined;
};
}



#endif // LINE_H
