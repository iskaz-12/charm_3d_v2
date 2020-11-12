//
// Created by zhrv on 12.11.2020.
//

#ifndef CHARM_3D_V2_POINT_H
#define CHARM_3D_V2_POINT_H
#include "global.h"
#include <cstring>

struct Point {
    union {
        struct {
            charm_real_t x,y,z;
        };
        charm_real_t coords[CHARM_DIM];
    };

    Point(): x(0.), y(0.), z(0.) {}

    Point(Point&p) {
        memcpy(coords, p.coords, sizeof(charm_real_t)*CHARM_DIM);
    }

    Point(charm_real_t _x, charm_real_t _y, charm_real_t _z): x(_x), y(_y), z(_z) {}

    inline charm_real_t& operator[] (charm_int_t i) {
        return coords[i];
    }

    inline const charm_real_t& operator[] (charm_int_t i) const {
        return coords[i];
    }

    inline Point& operator += (Point& p) {
        for (auto i: CHARM_DIM_RANGE) { coords[i] += p.coords[i]; } return *this;
    }

    inline Point& operator -= (Point& p) {
        for (auto i: CHARM_DIM_RANGE) { coords[i] -= p.coords[i]; } return *this;
    }

    inline Point& operator *= (charm_real_t a) {
        for (auto i: CHARM_DIM_RANGE) { coords[i] *= a; } return *this;
    }

    inline Point& operator /= (charm_real_t a) {
        for (auto i: CHARM_DIM_RANGE) { coords[i] /= a; } return *this;
    }

    friend charm_real_t scalar_prod(const Point& p1, const Point& p2) {
        charm_real_t res = 0.;
        for (auto i: CHARM_DIM_RANGE) {
            res += p1[i]*p2[i];
        }
    }

    friend Point vector_prod(const Point& p1, const Point& p2) {
        Point res;
        res.x = p1.y*p2.z-p1.z*p2.y;
        res.y = p1.z*p2.x-p1.x*p2.z;
        res.z = p1.x*p2.y-p1.y*p2.x;
        return res;
    }
};

typedef Point Vector;
#endif //CHARM_3D_V2_POINT_H
