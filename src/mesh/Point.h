/**
 * @author R.V.Zhalnin zhalnin@gmail.com
 */


#ifndef CHARM_3D_V2_POINT_H
#define CHARM_3D_V2_POINT_H
#include "global.h"
#include <cstring>
#include <cmath>

namespace charm {
    class Point {
    public:
        union {
            struct {
                Real x, y, z;
            };
            Real coords[DIM];
        };

        Point() : x(0.), y(0.), z(0.) {}

        Point(Point &p) {
            *this = p;
        }

        Point(const Point &p) {
            *this = p;
        }

        Point(Real _x, Real _y, Real _z) : x(_x), y(_y), z(_z) {}

        Point& operator = (const Point &p) {
            x = p.x;
            y = p.y;
            z = p.z;
            return *this;
        }

        inline Real &operator[](Int i) {
            return coords[i];
        }

        inline const Real &operator[](Int i) const {
            return coords[i];
        }

        inline Point &operator+=(Point &p) {
            for (auto i: {0,1,2}) { coords[i] += p.coords[i]; }
            return *this;
        }

        inline Point &operator-=(Point &p) {
            for (auto i: {0,1,2}) { coords[i] -= p.coords[i]; }
            return *this;
        }

        inline Point &operator*=(Real a) {
            for (auto i: {0,1,2}) { coords[i] *= a; }
            return *this;
        }

        inline Point &operator/=(Real a) {
            for (auto i: {0,1,2}) { coords[i] /= a; }
            return *this;
        }

        inline Real sqr() const {
            return x*x+y*y+z*z;
        }

        inline Real length() const {
            return ::sqrt(sqr());
        }

        friend Real scalarProd(const Point &p1, const Point &p2) {
            Real res = 0.;
            for (auto i: {0,1,2}) {
                res += p1[i] * p2[i];
            }
            return res;
        }

        friend Point vectorProd(const Point &p1, const Point &p2) {
            Point res;
            res.x = p1.y * p2.z - p1.z * p2.y;
            res.y = p1.z * p2.x - p1.x * p2.z;
            res.z = p1.x * p2.y - p1.y * p2.x;
            return res;
        }


    };


    typedef Array<Point> Points;

    typedef Points Vectors;
    typedef Point Vector;

}
#endif //CHARM_3D_V2_POINT_H
