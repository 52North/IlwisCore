#ifndef LINE_H
#define LINE_H

namespace Ilwis{
class Line2D  : public std::vector<Coordinate2d> {
public:
    Line2D(quint32 sz=0) : std::vector<Coordinate2d>(sz) {
    }

    Line2D& operator+=(const std::vector<double>& vec){
        if (!isValid() || vec.size() != 2)
            return *this;
        for(auto& pnt: *this) {
            pnt.x( pnt.x() + vec[0]);
            pnt.y( pnt.y() + vec[1]);
        }

        return *this;
    }

    Line2D& operator-=(const std::vector<double>& vec){
        if (!isValid() || vec.size() != 2)
            return *this;
        for(auto& pnt: *this) {
            pnt.x( pnt.x() - vec[0]);
            pnt.y( pnt.y() - vec[1]);
        }

        return *this;
    }

    Line2D& operator*=(const std::vector<double>& vec){
        if (!isValid() || vec.size()<=2 )
            return *this;
        double t1,t2;
        t1 = t2 = vec[0];
        if ( vec.size() == 2)
            t2 = vec[1];
        for(auto& pnt: *this) {
            pnt.x( pnt.x() * t1);
            pnt.y( pnt.y() * t2);
        }

        return *this;
    }

    Line2D& operator/=(const std::vector<double>& vec){
        if (!isValid() || vec.size()<=2 )
            return *this;
        double t1,t2;
        t1 = t2 = vec[0];
        if ( vec.size() == 2)
            t2 = vec[1];
        if ( t1 == 0 || t2 == 0)
            return *this;
        for(auto& pnt: *this) {
            pnt.x( pnt.x() / t1);
            pnt.y( pnt.y() / t2);
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

class Line3D  : public std::vector<Coordinate> {
public:
    Line3D(quint32 sz=0) : std::vector<Coordinate>(sz) {
    }

    Line3D& operator+=(const std::vector<double>& vec){
        if (!isValid() || vec.size() < 1 || vec.size() > 3)
            return *this;
        double t1,t2,t3;
        t1 = t2 = t3 = vec[0];
        if ( vec.size() >= 2)
            t2 = vec[1];
        if ( vec.size() >= 3)
            t3 = vec[2];
        for(auto& pnt: *this) {
            pnt.x( pnt.x() + t1);
            pnt.y( pnt.y() + t2);
            if ( valid3dCoord(t3, pnt))
                 pnt.z( pnt.z() + t3);
            else
                pnt.z(undefined);

        }

        return *this;
    }

    Line3D& operator-=(const std::vector<double>& vec){
        if (!isValid() || vec.size() < 1 || vec.size() > 3)
            return *this;
        double t1,t2,t3;
        t1 = t2 = t3 = vec[0];
        if ( vec.size() >= 2)
            t2 = vec[1];
        if ( vec.size() >= 3)
            t3 = vec[2];
        for(auto& pnt: *this) {
            pnt.x( pnt.x() + t1);
            pnt.y( pnt.y() + t2);
            if ( valid3dCoord(t3, pnt))
                 pnt.z( pnt.z() - t3);
            else
                pnt.z(undefined);
        }

        return *this;
    }

    Line3D& operator*=(const std::vector<double>& vec){
        if (!isValid() || vec.size()<=2 )
            return *this;
        if (!isValid() || vec.size() < 1 || vec.size() > 3)
            return *this;
        double t1,t2,t3;
        t1 = t2 = t3 = vec[0];
        if ( vec.size() >= 2)
            t2 = vec[1];
        if ( vec.size() >= 3)
            t3 = vec[2];
        for(auto& pnt: *this) {
            pnt.x( pnt.x() + t1);
            pnt.y( pnt.y() + t2);
            if ( valid3dCoord(t3, pnt))
                 pnt.z( pnt.z() * t3);
            else
                pnt.z(undefined);
        }

        return *this;
    }

    Line3D& operator/=(const std::vector<double>& vec){
        if (!isValid() || vec.size()<=2 )
            return *this;
        if (!isValid() || vec.size() < 1 || vec.size() > 3)
            return *this;
        double t1,t2,t3;
        t1 = t2 = t3 = vec[0];
        if ( vec.size() >= 2)
            t2 = vec[1];
        if ( vec.size() >= 3)
            t2 = vec[2];
        if ( t1 == 0 || t2 == 0 || t3 == 0)
            return *this;
        for(auto& pnt: *this) {
            pnt.x( pnt.x() + t1);
            pnt.y( pnt.y() + t2);
            if ( valid3dCoord(t3, pnt))
                 pnt.z( pnt.z() / t3);
            else
                pnt.z(undefined);

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
    bool valid3dCoord(double num, const Point3D<double>& pnt){
        return pnt.z() != undefined && num != undefined;

    }

    double undefined;
};
}



#endif // LINE_H
